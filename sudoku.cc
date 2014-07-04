#include <emscripten.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "Kinetic.h"

#include "tools/const.h"

using namespace std;

class SudokuLayout {
private:
  const int grid_x;
  const int grid_y;
  const int num_symbols;
  const int num_cells;
  vector<int *> region_array;
public:
  SudokuLayout(int _x, int _y, int _s) : grid_x(_x), grid_y(_y), num_symbols(_s), num_cells(_x*_y)
  {
    // @CAO For the moment, we're going to assume a standard 9x9 sudoku and load regions that way.
    // Rows first...
    region_array.push_back( (int[]) {0,1,2,3,4,5,6,7,8} );
    region_array.push_back( (int[]) {9,10,11,12,13,14,15,16,17} );
    region_array.push_back( (int[]) {18,19,20,21,22,23,24,25,26} );
    region_array.push_back( (int[]) {27,28,29,30,31,32,33,34,35} );
    region_array.push_back( (int[]) {36,37,38,39,40,41,42,43,44} );
    region_array.push_back( (int[]) {45,46,47,48,49,50,51,52,53} );
    region_array.push_back( (int[]) {54,55,56,57,58,59,60,61,62} );
    region_array.push_back( (int[]) {63,64,65,66,67,68,69,70,71} );
    region_array.push_back( (int[]) {72,73,74,75,76,77,78,79,80} );

    // Columns
    region_array.push_back( (int[]) {0,9,18,27,36,45,54,63,72} );
    region_array.push_back( (int[]) {1,10,19,28,37,46,55,64,73} ); 
    region_array.push_back( (int[]) {2,11,20,29,38,47,56,65,74} );
    region_array.push_back( (int[]) {3,12,21,30,39,48,57,66,75} ); 
    region_array.push_back( (int[]) {4,13,22,31,40,49,58,67,76} );
    region_array.push_back( (int[]) {5,14,23,32,41,50,59,68,77} );
    region_array.push_back( (int[]) {6,15,24,33,42,51,60,69,78} );
    region_array.push_back( (int[]) {7,16,25,34,43,52,61,70,79} );
    region_array.push_back( (int[]) {8,17,26,35,44,53,62,71,80} );

    // Box Regions
    region_array.push_back( (int[]) {0,1,2,9,10,11,18,19,20} );
    region_array.push_back( (int[]) {3,4,5,12,13,14,21,22,23} );
    region_array.push_back( (int[]) {6,7,8,15,16,17,24,25,26} );
    region_array.push_back( (int[]) {27,28,29,36,37,38,45,46,47} );
    region_array.push_back( (int[]) {30,31,32,39,40,41,48,49,50} );
    region_array.push_back( (int[]) {33,34,35,42,43,44,51,52,53} );
    region_array.push_back( (int[]) {54,55,56,63,64,65,72,73,74} );
    region_array.push_back( (int[]) {57,58,59,66,67,68,75,76,77} );
    region_array.push_back( (int[]) {60,61,62,69,70,71,78,79,80} );
  }
  ~SudokuLayout() { ; }

  int GetNumCells() const { return num_cells; }
  int GetNumRegions() const { return (int) region_array.size(); }
  int GetNumCols() const { return grid_x; }
  int GetNumRows() const { return grid_y; }

  int * GetRegion(int id) { return region_array[id]; }
  const int * GetRegion(int id) const { return region_array[id]; }

  int ToID(int col, int row) const { return row*grid_x + col; }
  int ToCol(int id) const { return id % grid_x; }
  int ToRow(int id) const { return id / grid_x; }
};


class SudokuCell {
private:
  int state;
  vector<bool> notes;
  bool warn;
  bool lock;
  vector<int> region_ids;
public:
  SudokuCell() : state(0), notes(10), lock(false) {
    for (int i = 1; i <= 9; i++) notes[i] = false;
  }
  SudokuCell(const SudokuCell & _in) : state(_in.state), notes(_in.notes), lock(_in.lock), region_ids(_in.region_ids)
  { ; }
  ~SudokuCell() { ; }
  
  int GetState() const { return state; }
  const vector<bool> & GetNotes() { return notes; }
  bool GetNote(int id) const { return notes[id]; }
  bool GetWarn() const { return warn; }
  bool GetLock() const { return lock; }
  int GetNumRegions() const { return (int) region_ids.size(); }
  const vector<int> & GetRegionIDs() { return region_ids; }
  int GetRegionID(int pos) { return region_ids[pos]; }
  
  void SetState(int _state) { state = _state; }
  void SetNote(int note_id, bool setting=true) { notes[note_id] = setting; }
  void ToggleNote(int note_id) { notes[note_id] = !notes[note_id]; }
  void SetWarn(bool setting=true) { warn = setting; }
  void SetLock(bool setting=true) { lock = setting; }

  void AddRegion(int id) { region_ids.push_back(id); }
};


class SudokuPuzzle {
private:
  const SudokuLayout & layout;
  vector<SudokuCell> cell_array;      // Info about the state of each cell in the puzzle
  vector<vector<int> > region_track; // Which values are set in each region?

public:
  SudokuPuzzle(const SudokuLayout & _layout)
    : layout(_layout)
    , cell_array(layout.GetNumCells())
    , region_track(layout.GetNumRegions())
  {
    const int num_regions = layout.GetNumRegions();
    const int region_size = layout.GetNumRows();
    for (int i = 0; i < num_regions; i++) {
      region_track[i].resize(10);
      for (int j = 1; j <= 9; j++) region_track[i][j] = 0;

      // Register cells as part of their regions.
      for (int j = 0; j < region_size; j++) {
        const int cur_cell_id = layout.GetRegion(i)[j];
        cell_array[cur_cell_id].AddRegion(i);
      }
    }
  }

  SudokuPuzzle(const SudokuPuzzle & _in)
    : layout(_in.layout)
    , cell_array(_in.cell_array)
    , region_track(_in.region_track)
  {
  }

  ~SudokuPuzzle() { ; }

  const SudokuLayout & GetLayout() { return layout; }
  int GetState(int cell_id) const { return cell_array[cell_id].GetState(); }
  int GetState(int col, int row) const { return GetState(layout.ToID(col, row)); }
  bool GetWarn(int cell_id) const { return cell_array[cell_id].GetWarn(); }
  bool GetWarn(int col, int row) const { return GetWarn(layout.ToID(col, row)); }
  bool GetLock(int cell_id) const { return cell_array[cell_id].GetLock(); }
  bool GetLock(int col, int row) const { return GetLock(layout.ToID(col, row)); }
  bool GetNote(int cell_id, int value) const { return cell_array[cell_id].GetNote(value); }
  bool GetNote(int col, int row, int value) const { return GetNote(layout.ToID(col, row), value); }
  
  void SetState(int cell_id, int value) {
    SudokuCell & cur_cell = cell_array[cell_id];

    for (int i = 0; i < cur_cell.GetNumRegions(); i++) {
      region_track[ cur_cell.GetRegionID(i) ][cur_cell.GetState()]--; // Reduce old value in current region.
      region_track[ cur_cell.GetRegionID(i) ][value]++;               // Increase new value in current region.
    }
    cur_cell.SetState(value);
  }
  void SetState(int col, int row, int value) { SetState(layout.ToID(col,row), value); }

  void SetNote(int cell_id, int value, bool setting=true) {
    cell_array[cell_id].SetNote(value, setting);
  }
  void ToggleNote(int cell_id, int value) {
    cell_array[cell_id].ToggleNote(value);
  }
  void Lock(int cell_id) { cell_array[cell_id].SetLock(true); }

  void LockAllSet() {
    for (int i = 0; i < layout.GetNumCells(); i++) {
      if (cell_array[i].GetState() > 0) cell_array[i].SetLock(true);
    }
  }
};



/////////////////////////////
//  Track move information
/////////////////////////////

class PuzzleMove {
public:
  enum Type { VALUE, NOTE, AUTONOTES, BOOKMARK };

private:
  Type type;      // What type of move was this?
  int id;         // ID of cell affected.
  int new_state;  // New state of affected cell, if relevant.
  int prev_state; // Previous state of affected cell, if relevant.
  bool next;      // Is this move clustered with others for a single undo?

public:
  PuzzleMove(Type _type, int _id, int _new_state, int _prev_state)
    : type(_type), id(_id), new_state(_new_state), prev_state(_prev_state) { ; }
  ~PuzzleMove() { ; }
};





///////////////////////////
// Build the Interface!!
///////////////////////////

class SudokuInterface {
private:
  SudokuPuzzle puzzle;

  // Configurable info
  int min_size;      // Minimum puzzle size with resizing
  int cell_padding;  // Pedding between edge of cell and internal number.
  int border;        // Thickness of outer border.
  int mid_width;     // Thickness of border between 3x3 square regions.
  int thin_width;    // Thickness of normal cell walls.

  // Calculated info
  int puzzle_size;    // Wdith (and height) for overall puzzle
  int cell_width;     // Width (and height) of an individual grid cell.
  int region_width;   // Width (and height) of a 3x3 region
  int board_width;    // Width (and height) of the entire board
  int full_width;     // Overall width of entire board, include borders.

  int puz_text_pt;    // Font size for text filling a cell.
  int puz_note_width; // Space for smaller (note) characters in each cell
  int puz_note_pt;    // Font size for note characters

  int button_w;           // Width of buttons.
  int button_r;           // Corner radious of buttons.
  vector<int> buttons_x;  // X position of buttons.
  int buttons_y;          // All buttons have the same y.

  // Graphical elements
  emkCustomShape puzzle_board; // @CAO Should this be an emkRect to capture mouse clicks?
  emkLayer layer_main;
  emkLayer layer_buttons;
  emkLayer layer_tooltips;
  emkStage stage;

  // Current state
  int cell_id;                   // Current cell in focus
  int highlight_id;              // Is there a cell we should be highlighting?
  int hover_val;                 // The note value being hovered over...
  int bm_level;                  // Number of bookmarks set so far.
  vector<vector<int> > bm_touch; // How many times has each cell been touched in each bookmark level?
  bool do_warnings;              // Should we warn when there is an obvious error?
  bool do_autonotes;             // Should we automatically provide notes?
  bool toggle_click;             // Should clicks be for autonotes by default?
    
public:
  SudokuInterface(const SudokuPuzzle & _puzzle)
    : puzzle(_puzzle)
    , min_size(500), cell_padding(5), border(8), mid_width(4), thin_width(2)
    , buttons_x(9)
    , puzzle_board(border, border, this, &SudokuInterface::DrawGrid)
    , stage(1200, 600, "container")
    , cell_id(0), highlight_id(-1), hover_val(-1), bm_level(0), bm_touch(81)
    , do_warnings(false), do_autonotes(false), toggle_click(false)
  {
    stage.ResizeMax(min_size, min_size);
    ResizeBoard();

    puzzle_board.On("click", this, &SudokuInterface::OnClick);

    layer_main.Add(puzzle_board);
    stage.Add(layer_main);
    stage.Add(layer_buttons);
    stage.Add(layer_tooltips);
  }
  ~SudokuInterface() { ; }

  void OnClick(const emkEventInfo & info) {
    emkAlert(info.shift_key);
  }

  void ResizeBoard() {
    int win_size = std::min((int)(stage.GetX() * 1.25), stage.GetY()); // Use the window size.
    win_size = std::max(win_size, min_size); // However, don't let puzzle get too small.
    puzzle_size = win_size * 0.8;

    // These are dynamically sized numbers.
    cell_width = (puzzle_size - border*2)/9;
    region_width = cell_width * 3;
    board_width = cell_width * 9;

    puz_text_pt = cell_width - 2 * cell_padding;     // Font size for text filling a cell.
    puz_note_width = cell_width / 3 - 1;             // Space for note characters in each cell
    puz_note_pt = puz_note_width - 1;           // Font size for smaller characters
    full_width = board_width + border*2;

    button_w = full_width/10;     // Width of each button
    button_r = button_w/5;        // Corner radius of each button
    const int offset = 3;

    buttons_x[0] = offset;
    buttons_x[1] = offset + button_w;
    buttons_x[2] = offset + button_w * 2;
    buttons_x[3] = offset + button_w * 3;
    buttons_x[4] = offset + button_w * 4;
    buttons_x[5] = button_w * 5.5;
    buttons_x[6] = button_w * 7 - offset;
    buttons_x[7] = button_w * 8 - offset;
    buttons_x[8] = button_w * 9 - offset;
    buttons_y = puzzle_size + 2;    // Y-value of main button bar    
  }

  void DrawGrid(emkCanvas & canvas) {
    const int offset = border; // @CAO Technically we should deal with an x & y here...

    const int active_col = puzzle.GetLayout().ToCol(cell_id);
    const int active_row = puzzle.GetLayout().ToRow(cell_id);
    const int cell_x = offset + cell_width * active_col;
    const int cell_y = offset + cell_width * active_row;

    // Clear out the background
    canvas.SetFillStyle(emk::Color(255,250,245));
    canvas.Rect(offset, offset, board_width, board_width, true);

    // Highlight the regions affected.
    if (active_row != -1 && active_col != -1) {
      canvas.SetFillStyle(emk::Color(200, 200, 250, 0.5));  // Highlight color
      canvas.Rect(cell_x, offset, cell_width, board_width, true);
      canvas.Rect(offset, cell_y, board_width, cell_width, true);
 	 
      const int region_x = active_col / 3;
      const int region_y = active_row / 3;
      canvas.Rect(offset + region_x*region_width, offset + region_y*region_width, region_width, region_width, true);

      canvas.SetFillStyle(emk::Color(250, 250, 250));     // Target (white)
      canvas.Rect(cell_x, cell_y, cell_width, cell_width, true);
        
      // If there is a cell to highlight, do so.
      if (highlight_id != -1) {
        const int h_col = puzzle.GetLayout().ToCol(highlight_id);
        const int h_row = puzzle.GetLayout().ToRow(highlight_id);
        const int hx = offset + cell_width * h_col;
        const int hy = offset + cell_width * h_row;
        canvas.SetFillStyle("yellow");
        canvas.Rect(hx, hy, cell_width, cell_width, true);
      }
    }

    // Setup drawing the actual grid.
    canvas.BeginPath();
    canvas.SetFillStyle("black");
    canvas.SetLineJoin("round");
    canvas.SetLineWidth(mid_width);
      
    // Draw region borders
    canvas.Rect(offset + region_width, offset/2, region_width, board_width+offset);
    canvas.Rect(offset/2, offset + region_width, board_width+offset, region_width);

    // Draw thin lines
    canvas.SetLineWidth(thin_width);
    for (int i = 1; i < 9; i++) {
      const int x = i*cell_width + offset;
      const int y = i*cell_width + offset;
      canvas.MoveTo(x, offset);
      canvas.LineTo(x, offset + board_width);	  
      canvas.MoveTo(offset, y);
      canvas.LineTo(offset + board_width, y);	  
    }
    canvas.Stroke();
 
    // Outer box
    canvas.SetLineWidth(border);
	    
    const int corner_radius = 10;
    const int dist1 = offset/2 + corner_radius;
    const int dist2 = board_width + 3 * offset/2 - corner_radius;

    canvas.BeginPath();
    canvas.MoveTo(dist1 - corner_radius, dist1);
    canvas.Arc(dist1, dist1, corner_radius, emk::PI, 3*emk::PI/2, false);
    canvas.Arc(dist2, dist1, corner_radius, 3*emk::PI/2, 0, false);
    canvas.Arc(dist2, dist2, corner_radius, 0, emk::PI/2, false);
    canvas.Arc(dist1, dist2, corner_radius, emk::PI/2, emk::PI, false);
    canvas.ClosePath();
    canvas.Stroke();

    canvas.BeginPath();

    // Setup this bookmark level for coloring.
    const vector<int> & bm_touch_cur = bm_touch[bm_level];

    // Fill in the values
    canvas.SetTextAlign("center");
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (puzzle.GetState(i,j) == 0) { // No value, so print notes!
          const int cur_x = offset + cell_width * i;
          const int cur_y = offset + cell_width * j;

          canvas.SetFont(std::to_string(puz_note_pt) + "pt Calibri");
          // Print the values from 1 to 9.
          for (int mi = 0; mi < 3; mi++) {
            const int text_x = cur_x + cell_width * (0.30 * (double) mi + 0.20);
            for (int mj = 0; mj < 3; mj++) {
              const int val = mi + 1 + (mj*3);
              if (puzzle.GetNote(i,j,val) == false) continue;
          	  
              if (do_autonotes) canvas.SetFillStyle("#00AA00");
              else canvas.SetFillStyle("#006600");
              const int text_y = cur_y + puz_note_width * (mj+1);
              canvas.Text(to_string(val), text_x, text_y);
            }
          }	  
        }
        else { // This cell has a value, so we should draw it!
          canvas.SetFont(std::to_string(puz_text_pt) + "pt Calibri");
          if (puzzle.GetLock(i,j) != 0) canvas.SetFillStyle("black");    // This number is locked! (original in puzzle)
          else {                                                         // Changeable cell.
            const int cur_id = puzzle.GetLayout().ToID(i, j);

            if (puzzle.GetWarn(cur_id) == true) {                        // Are warnings on for this cell?
              // ...Has cell changed on the current bookmark level?
              if (bm_touch_cur[cur_id] > 0) canvas.SetFillStyle("#880000");
              else canvas.SetFillStyle("#AA6666");
            }
            else {                                                       // No warnings
              // ...Has cell changed on the current bookmark level?
              if (bm_touch_cur[cur_id] > 0) canvas.SetFillStyle("#0000FF");
              else canvas.SetFillStyle("#6666AA");
            }
          }
          canvas.Text(std::to_string(puzzle.GetState(i,j)),
                      offset + cell_width * i + cell_width/2,
                      offset + cell_width * (j+1) - cell_padding);
        }
      }
    }


      // Give options for target cell.
      if (cell_id != -1 && puzzle.GetState(cell_id) == 0) {
        canvas.SetFont(std::to_string(puz_text_pt) + "pt Calibri");
        // Print the values from 1 to 9.
        for (int i = 0; i < 3; i++) {
          const int text_x = cell_x + cell_width * (0.30 * i + 0.20);
          for (int j = 0; j < 3; j++) {
            const int val = i + 1 + (j*3);
            if (puzzle.GetNote(cell_id, val) == false) {
              if (val == hover_val) canvas.SetFillStyle("#FF8800");
              else canvas.SetFillStyle("#888888");
            } else {
              if (val == hover_val) canvas.SetFillStyle("#884400");
              else canvas.SetFillStyle("#008800");
            }
            const int text_y = cell_y + puz_note_width * (j+1);
            canvas.Text(std::to_string(val), text_x, text_y);
          }
        }	  
      }

      // Mark this region as being click-on-able.
      canvas.BeginPath();
      canvas.MoveTo(offset, offset);
      canvas.LineTo(offset + board_width, offset);
      canvas.LineTo(offset + board_width, offset + board_width);
      canvas.LineTo(offset, offset + board_width);
      canvas.ClosePath();
      canvas.SetupTarget(puzzle_board);
  }
};


/////@@@@@@@@@@@@@@@@@@
/*

  // Track the basic puzzle information.
  this.info = {

    // Undo/redo stacks
    undo_stack: [],
    redo_stack: [],
    

  };


  this.puzzle_board.on('mousedown', function(evt) {
    // If this cell is locked, stop here -- there's nothing to do.
    if (this.puzzle.GetLock(this.cell_id) == 1) return;

    // If this cell was previously set, clear its current state.
    if (this.puzzle.GetState(this.cell_id) > 0) {
      this.info.interface.SetState(this.cell_id, 0);      
    }

    // Otherwise, see which value is being clicked on and set the state.
    else {
      // Shift indicates that pencil-marks should be done instead of setting the value.
      shift_key = evt.shiftKey;
      if (this.info.toggle_click == true) shift_key = !shift_key;

      if (shift_key == false) {
        this.info.interface.SetState(this.cell_id, this.hover_val);
      } else {
        this.info.interface.ToggleNote(this.cell_id, this.hover_val);
      }
    }
    
    this.getLayer().draw();  
  });

  this.puzzle_board.on('mousemove', function(evt) {    
    var rel_x = (evt.layerX - this.info.border)/this.info.cell_width;
    var rel_y = (evt.layerY - this.info.border)/this.info.cell_width;

    this.cell_id = this.puzzle.layout.ToID(Math.floor(rel_x), Math.floor(rel_y));
    
    var cell_x = rel_x - Math.floor(rel_x);
    var cell_y = rel_y - Math.floor(rel_y);

    this.hover_val = Math.floor(cell_x * 3) + 3*Math.floor(cell_y * 3) + 1;

    this.getLayer().draw();
  });


////////////////////////////////////
//  Button Callbacks
////////////////////////////////////

  this.DoRewind = function() {
    var step = 0, bm_index = -1, last_move;
    for (var i=this.info.undo_stack.length - 1; i >= 0; i--) {
      if (my_sudoku.info.undo_stack[i].type == 'bookmark') {
        bm_index = i;
        break;
      }
      last_move = my_sudoku.info.undo_stack[i];
      window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoUndo();},100*step);      
      step++;
    }
    my_sudoku.info.highlight_id = last_move.id;
    window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoRedo();},100*(step+2));
    window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoUndo();},100*(step+4));
    window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoRedo();},100*(step+6));
    window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoUndo();},100*(step+8));
    window.setTimeout(function(){ if (my_sudoku.info.undo_stack.length > 0) my_sudoku.DoUndo();},100*(step+10));
  }

  this.DoUndo = function() {
    // DEBUG!
    if (this.info.undo_stack.length == 0) {
      alert('Error! Undo called with no actions available!');
      return;
    }
    
    // Grab the last action done.
    var undo_item = this.info.undo_stack.pop();
    var value_id = -1;

    // Move it to the redo stack
    this.info.redo_stack.push(undo_item);

    // Undo each associated action.
    while (undo_item !== undefined) {
      // Reverse this item.
      if (undo_item.type == 'value') {
        touched_id = undo_item.id;
        this.puzzle.SetState(undo_item.id, undo_item.prev_state);
      } else if (undo_item.type == 'note') {
        this.puzzle.ToggleNote(undo_item.id, undo_item.new_state);
      } else if (undo_item.type == 'bookmark') {
        this.info.bm_level--;
      } else if (undo_item.type == 'autonotes') {
        this.info.do_autonotes = !this.info.do_autonotes;
        this.button_bar.autonotes.toggle_on = !this.button_bar.autonotes.toggle_on;
      } else {
        alert('Error: Unknown type "' + undo_item.type + '" in undo stack!');
      }

      undo_item = undo_item.next;
    }
    
    // Update buttons that have to do with the undo and redo stacks.
    if (this.info.undo_stack.length == 0) {      // If the undo stack is empty, gray-out related buttons
      this.button_bar.rewind.inactive = true;
      this.button_bar.undo.inactive = true;
      this.button_bar.bookmark.inactive = true;
    }
    else if (this.info.undo_stack.slice(-1)[0].type == 'bookmark') {
      this.button_bar.bookmark.inactive = true;    // Gray-out the bookmark button
    }
    else {
      this.button_bar.bookmark.inactive = false;   // Make sure bookmark button is usable.
    }

    this.button_bar.redo.inactive = false;
    this.button_bar.redoall.inactive = false;
    
    // Update bookmark tracking...
    if (value_id != -1) {
      var bm_level = this.info.bm_touch[this.info.bm_level];
      bm_level[value_id]--;
    }
    
    // Update Warnings
    if (this.info.do_warnings == true) this.UpdateWarnings();

    // And redraw everything that's changed.
    this.layer_main.draw();
    this.layer_buttons.draw();
  }

  this.DoBookmark = function() {
    this.info.bm_level++;
    this.SetupBookmarkLevel(this.info.bm_level);

    // Put this move into the undo stack.
    var move_info = new cPuzzleMove('bookmark', -1, -1, -1, -1);
    this.info.undo_stack.push(move_info);

    // Gray-out the bookmark
    this.button_bar.bookmark.inactive = true;

    this.layer_main.draw();
  }

  this.DoRedo = function() {
    // DEBUG!
    if (this.info.redo_stack.length == 0) {
      alert('Error! Redo called with no actions available.  Canceling');
      return;
    }
    
    var redo_item = this.info.redo_stack.pop();
    var value_id = -1;
    var is_bookmark = false;
    
    // Move it to the undo stack
    this.info.undo_stack.push(redo_item);

    // Redo each item in this group.
    while (redo_item !== undefined) {
      if (redo_item.type == 'value') {
        this.puzzle.SetState(redo_item.id, redo_item.new_state);
        value_id = redo_item.id;
      } else if (redo_item.type == 'note') {
        this.puzzle.ToggleNote(redo_item.id, redo_item.new_state);      
      } else if (redo_item.type == 'bookmark') {
        this.info.bm_level++;
        is_bookmark = true;
        this.SetupBookmarkLevel(this.info.bm_level);
      } else if (redo_item.type == 'autonotes') {
        this.info.do_autonotes = !this.info.do_autonotes;
        this.button_bar.autonotes.toggle_on = !this.button_bar.autonotes.toggle_on;
      } else {
        alert('Error: Unknown type "' + undo_item.type + '" in undo stack!');
      }

      redo_item = redo_item.next;
    }
        
    // Update buttons that have to do with the undo and redo stacks.
    this.button_bar.rewind.inactive = false;
    this.button_bar.undo.inactive = false;
    this.button_bar.bookmark.inactive = false;
    if (this.info.redo_stack.length == 0) {
      this.button_bar.redo.inactive = true;
      this.button_bar.redoall.inactive = true;
    }
    
    // Track with this bookmark info.
    if (value_id != -1) {
      var bm_level = this.info.bm_touch[this.info.bm_level];
      bm_level[value_id]++;
    }
    
    // If it is a bookmark, gray the bookmark button.
    if (is_bookmark) {
      this.button_bar.bookmark.inactive = true;
    }

    // Update Warnings
    if (this.info.do_warnings == true) this.UpdateWarnings();

    // And redraw everything that's changed.
    this.layer_main.draw();
    this.layer_buttons.draw();
  }

  this.DoRedoall = function() {
    for (var i=0; i < this.info.redo_stack.length; i++) {
      window.setTimeout(function(){ if (my_sudoku.info.redo_stack.length > 0) my_sudoku.DoRedo();},100*i);
    }
  }

  this.DoHint = function() {
    // @CAO Write this!
    alert('Hint Called!');
  }

  this.DoWarnings = function() {
    this.info.do_warnings = !this.info.do_warnings;
    if (this.info.do_warnings == true) this.UpdateWarnings();
    else this.ClearWarnings();
    this.layer_main.draw();
  }

  this.DoAutonotes = function() {
    var move_info = new cPuzzleMove('autonotes', -1, -1, -1, -1);
    this.info.undo_stack.push(move_info);
    this.info.do_autonotes = !this.info.do_autonotes;
    if (this.info.do_autonotes == true) this.UpdateAutonotes();
    this.layer_main.draw();
  }

  this.DoToggleclick = function() {
    this.info.toggle_click = !(this.info.toggle_click);
  }



////////////////////////////////////////
//  Build the button bar INFASTRUCTURE
////////////////////////////////////////

  this.button_bar = {};

  this.button_names = {
    rewind: 0,
    undo: 1,
    bookmark: 2,
    redo: 3,
    redoall: 4,
    autonotes: 5,
    hint: 6,
    warnings: 7,
    toggleclick: 8
  }

  for (cur_button in this.button_names) {
    this.button_bar[cur_button] = new Kinetic.Shape({
      drawFunc: function(canvas) {
        this.canvas = canvas.getContext();
 
        this.x = this.info.buttons_x[this.name];
        this.y = this.info.buttons_y;
        this.x2 = this.x + this.info.button_w;
        this.y2 = this.y + this.info.button_w;
        this.x_in = this.x + this.info.buttons_r;
        this.y_in = this.y + this.info.buttons_r;
        this.x2_in = this.x2 - this.info.buttons_r;
        this.y2_in = this.y2 - this.info.buttons_r;
  
        // Set the button color
        if (this.mouse_down == true) this.canvas.fillStyle = 'blue';
  	    else if (this.mouse_over == true) {
  	      if (this.toggle_on == true) this.canvas.fillStyle = 'rgb(250,250,200)';
  	      else this.canvas.fillStyle = 'rgb(240,240,255)';
  	    }
  	    else {
  	      if (this.toggle_on == true) this.canvas.fillStyle = 'rgb(255,255,100)';
  	      else this.canvas.fillStyle = 'rgb(255,250,245)';
  	    }
  
  	    this.canvas.beginPath();
  	    if (this.toggle_on == true) this.canvas.lineWidth = 4;
  	    else this.canvas.lineWidth = 2;
  	    if (this.round_corners.ul == true) {
    	    this.canvas.moveTo(this.x, this.y_in);
    	    this.canvas.arc(this.x_in, this.y_in, this.info.buttons_r, emk::PI, 3*emk::PI/2, false);
    	  } else this.canvas.moveTo(this.x, this.y);
    	  if (this.round_corners.ur == true) {
    	    this.canvas.arc(this.x2_in, this.y_in, this.info.buttons_r, 3*emk::PI/2, 0, false);      	  
    	  } else this.canvas.lineTo(this.x2, this.y);
    	  if (this.round_corners.lr == true) {
    	    this.canvas.arc(this.x2_in, this.y2_in, this.info.buttons_r, 0, emk::PI/2, false);      	  
  	    } else this.canvas.lineTo(this.x2, this.y2);
    	  if (this.round_corners.ll == true) {
    	    this.canvas.arc(this.x_in, this.y2_in, this.info.buttons_r, emk::PI/2, emk::PI, false);
  	    } else this.canvas.lineTo(this.x, this.y2);
  	    this.canvas.closePath();
  	    this.canvas.fill();
  	    this.canvas.stroke();
  
      	// Draw the appropriate icon.
      	// First, shift the icon to be on a 100x100 grid, and shift back afterward.
      	this.canvas.save();
        this.canvas.translate(this.x+5, this.y+5);
        this.canvas.scale((this.info.button_w - 10)/100, (this.info.button_w - 10)/100);    
  	    this.DrawIcon();
  	    //this.canvas.setTransform(1, 0, 0, 1, 0, 0);
  	    this.canvas.restore();
  
  	    // Make the button clickable (or grayed out!)
  	    this.canvas.beginPath();
  	    this.canvas.lineWidth = 2;
  	    if (this.round_corners.ul == true) {
    	    this.canvas.moveTo(this.x, this.y_in);
    	    this.canvas.arc(this.x_in, this.y_in, this.info.buttons_r, emk::PI, 3*emk::PI/2, false);
    	  } else this.canvas.moveTo(this.x, this.y);
    	  if (this.round_corners.ur == true) {
    	    this.canvas.arc(this.x2_in, this.y_in, this.info.buttons_r, 3*emk::PI/2, 0, false);      	  
    	  } else this.canvas.lineTo(this.x2, this.y);
    	  if (this.round_corners.lr == true) {
    	    this.canvas.arc(this.x2_in, this.y2_in, this.info.buttons_r, 0, emk::PI/2, false);      	  
  	    } else this.canvas.lineTo(this.x2, this.y2);
    	  if (this.round_corners.ll == true) {
    	    this.canvas.arc(this.x_in, this.y2_in, this.info.buttons_r, emk::PI/2, emk::PI, false);
  	    } else this.canvas.lineTo(this.x, this.y2);
  	    this.canvas.closePath();
  	    if (this.inactive == true) {
    	    this.canvas.fillStyle = 'rgba(200,200,200,0.5)';
    	    this.canvas.fill();
  	    }
  	    canvas.fillStroke(this);
      }
    });
    this.button_bar[cur_button].name = cur_button;
    
    this.button_bar[cur_button].info = this.info;
    this.button_bar[cur_button].mouse_down = false;
    this.button_bar[cur_button].mouse_over = false;
    this.button_bar[cur_button].toggle = false;
    this.button_bar[cur_button].toggle_on = false;
    this.button_bar[cur_button].inactive = false;
    this.button_bar[cur_button].round_corners = {ul:false, ur:false, ll:false, lr:false};

    this.button_bar[cur_button].UpdateHover = function() {
      // This is a stub to be filled in for each button as need.
      // In particular, you may want to adjust:
      // * Highlight in grid (this is automatically turned off when you leave the hover)
      // * Tool-tip message.
    }
    
    this.button_bar[cur_button].on('mousedown', function(evt) {
      if (this.inactive == true) return;
      this.mouse_down = true;
      this.getLayer().draw();
    });
  
    this.button_bar[cur_button].on('mouseup', function(evt) {
      if (this.inactive == true) return;
      this.mouse_down = false;
      if (this.toggle) this.toggle_on = !(this.toggle_on);
      this.Trigger();
      this.UpdateHover();
      this.getLayer().draw();
      this.info.interface.layer_main.draw();
      this.info.interface.UpdateTooltip(this.x, this.y, this.tooltip);
    });
    
    this.button_bar[cur_button].on('mouseenter', function(evt) {
      this.mouse_over = true;
      this.UpdateHover();
      this.getLayer().draw();
      this.info.interface.layer_main.draw();
      this.info.interface.UpdateTooltip(this.x, this.y, this.tooltip);
    });
  
    this.button_bar[cur_button].on('mouseleave', function(evt) {
      this.mouse_over = false;
      this.mouse_down = false;
      this.info.highlight_id = -1;
      this.getLayer().draw();
      this.info.interface.layer_main.draw();
      this.info.interface.HideTooltip();
    });
  }

///////////////////////
//  Button Finishing!
///////////////////////

  // Setup rounded corners
  this.button_bar.rewind.round_corners = {ul:true, ur:false, ll:true, lr:false};
  this.button_bar.redoall.round_corners = {ul:false, ur:true, ll:false, lr:true};
  this.button_bar.hint.round_corners = {ul:true, ur:true, ll:true, lr:true};
  this.button_bar.warnings.round_corners = {ul:true, ur:false, ll:true, lr:false};
  this.button_bar.toggleclick.round_corners = {ul:false, ur:true, ll:false, lr:true};

  // Setup toggle buttons
  this.button_bar.warnings.toggle = true;
  this.button_bar.autonotes.toggle = true;
  this.button_bar.toggleclick.toggle = true;

  // Setup inactive buttons
  this.button_bar.rewind.inactive = true;
  this.button_bar.undo.inactive = true;
  this.button_bar.bookmark.inactive = true;
  this.button_bar.redo.inactive = true;
  this.button_bar.redoall.inactive = true;

  // Setup tooltips
  this.button_bar.rewind.tooltip = 'Rewind to bookmark';
  this.button_bar.undo.tooltip = 'Undo';
  this.button_bar.bookmark.tooltip = 'Add bookmark';
  this.button_bar.redo.tooltip = 'Redo';
  this.button_bar.redoall.tooltip = 'Redo ALL';
  this.button_bar.hint.tooltip = 'Get HINT';
  this.button_bar.warnings.tooltip = 'Toggle highlighting of conflicts';
  this.button_bar.autonotes.tooltip = 'Toggle automatic pencilmarks';
  this.button_bar.toggleclick.tooltip = 'Toggle answers & pencilmarks';

  // Setup hover tooltips and highlights.
  this.button_bar.undo.UpdateHover = function() {
    if (this.info.undo_stack.length == 0) {
      this.info.highlight_id = -1;
      this.tooltip = "Nothing to UNDO";
      return;
    }
    var stack_top = this.info.undo_stack.slice(-1)[0];
    if (stack_top.type == 'value') {
      this.info.highlight_id = stack_top.id;
      this.tooltip = "UNDO value = " + stack_top.new_state;
    }
    if (stack_top.type == 'note') {
      this.info.highlight_id = stack_top.id;
      this.tooltip = "UNDO pencilmark toggle " + stack_top.new_state;
    }
    if (stack_top.type == 'bookmark') {
      this.tooltip = "UNDO bookmark";      
      this.info.highlight_id = -1;
    }
    if (stack_top.type == 'autonotes') {
      this.tooltip = "UNDO pencil marks";      
      this.info.highlight_id = -1;
    }
  }

  this.button_bar.bookmark.UpdateHover = function() {
    if (this.info.undo_stack.length == 0) {
      this.tooltip = "Start already has BOOKMARK";
      return;
    }  
    if (this.info.undo_stack.slice(-1)[0].type == 'bookmark') {      
      this.tooltip = "Already at BOOKMARK";
      return;
    }
    this.tooltip = "Add BOOKMARK";
  }

  this.button_bar.redo.UpdateHover = function() {
    if (this.info.redo_stack.length == 0) {
      this.info.highlight_id = -1;
      this.tooltip = "Nothing to REDO";
      return;
    }
    var stack_top = this.info.redo_stack.slice(-1)[0];
    if (stack_top.type == 'value') {
      this.info.highlight_id = stack_top.id;
      this.tooltip = "REDO value = " + stack_top.new_state;
    }
    if (stack_top.type == 'note') {
      this.info.highlight_id = stack_top.id;
      this.tooltip = "REDO pencilmark toggle " + stack_top.new_state;
    }
    if (stack_top.type == 'bookmark') {
      this.tooltip = "REDO bookmark";      
      this.info.highlight_id = -1;
    }
    if (stack_top.type == 'autonotes') {
      this.tooltip = "REDO pencil marks";      
      this.info.highlight_id = -1;
    }
  }
  // Setup function calls
  this.button_bar.rewind.Trigger      = function() { this.info.interface.DoRewind(); }
  this.button_bar.undo.Trigger        = function() { this.info.interface.DoUndo(); }
  this.button_bar.bookmark.Trigger    = function() { this.info.interface.DoBookmark(); }
  this.button_bar.redo.Trigger        = function() { this.info.interface.DoRedo(); }
  this.button_bar.redoall.Trigger     = function() { this.info.interface.DoRedoall(); }
  this.button_bar.hint.Trigger        = function() { this.info.interface.DoHint(); }
  this.button_bar.warnings.Trigger    = function() { this.info.interface.DoWarnings(); }
  this.button_bar.autonotes.Trigger   = function() { this.info.interface.DoAutonotes(); }
  this.button_bar.toggleclick.Trigger = function() { this.info.interface.DoToggleclick(); }

  // Setup icons
  // Note: All icons are adjusted to be on a 100x100 grid.
  
  this.button_bar.rewind.DrawIcon = function() {
    // Draw the bookmark
    this.canvas.beginPath();
    this.canvas.lineWidth = 3;
    this.canvas.fillStyle = '#F5DEB3';
    this.canvas.moveTo(5, 100);
    this.canvas.arc(15, 10, 10, emk::PI, emk::PI*1.5);
    this.canvas.arc(35, 10, 10, emk::PI*1.5, 0);
    this.canvas.lineTo(45, 100);
    this.canvas.lineTo(25, 90);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 3;
    this.canvas.shadowOffsetY = 3;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();

    // Draw Arrow 1
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(45, 30);
    this.canvas.lineTo(65, 10);
    this.canvas.lineTo(65, 20);
    this.canvas.lineTo(95, 20);
    this.canvas.lineTo(95, 40);
    this.canvas.lineTo(65, 40);
    this.canvas.lineTo(65, 50);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 3;
    this.canvas.shadowOffsetY = 3;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
    
    // Draw Arrow 2  
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(45, 75);
    this.canvas.lineTo(65, 55);
    this.canvas.lineTo(65, 65);
    this.canvas.lineTo(95, 65);
    this.canvas.lineTo(95, 85);
    this.canvas.lineTo(65, 85);
    this.canvas.lineTo(65, 95);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 3;
    this.canvas.shadowOffsetY = 3;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
  }

  this.button_bar.undo.DrawIcon = function() {
    this.canvas.beginPath();
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(30,95);              // Arrow point
    this.canvas.lineTo(50,75);               // Angle up on right side of arrow
    this.canvas.lineTo(40,75);               // Back in to base of arrow head
    this.canvas.lineTo(40,45);               // Up to bend in arrow
    this.canvas.arc(55, 45, 15, emk::PI, 0); // Lower bend
    this.canvas.lineTo(70,95);
    this.canvas.lineTo(90,95);
    this.canvas.lineTo(90,45);
    this.canvas.arc(55, 45, 35, 0, emk::PI, true);
    this.canvas.lineTo(20,75);
    this.canvas.lineTo(10,75);

    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();    
  }
  
  this.button_bar.bookmark.DrawIcon = function() {
    // Grid
    var gridx = 5;
    var gridy = 10;
    var gridw = 90;
    var gridh = 90;
    var grid_lines = 6;
    var grid_step = gridw / (grid_lines - 1);

    var gridx2 = gridx + gridw;
    var gridy2 = gridy + gridh;
    
    this.canvas.beginPath();
    this.canvas.lineWidth = 3;
    this.canvas.fillStyle = 'white';
    this.canvas.rect(gridx, gridy, gridw, gridh);
    this.canvas.fill();
    for (var i = 0; i < grid_lines; i++) {
	    this.canvas.moveTo(gridx,             gridy+grid_step*i);
	    this.canvas.lineTo(gridx2,            gridy+grid_step*i);
	    this.canvas.moveTo(gridx+grid_step*i, gridy);
	    this.canvas.lineTo(gridx+grid_step*i, gridy2);
    }
    this.canvas.stroke();    

    this.canvas.beginPath();
    this.canvas.lineWidth = 3;
    this.canvas.fillStyle = '#F5DEB3';
    this.canvas.moveTo(15, 100);
    this.canvas.arc(25, 10, 10, emk::PI, emk::PI*1.5);
    this.canvas.arc(45, 10, 10, emk::PI*1.5, 0);
    this.canvas.lineTo(55, 100);
    this.canvas.lineTo(35, 90);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 3;
    this.canvas.shadowOffsetY = 3;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
  }

  this.button_bar.redo.DrawIcon = function() {
    this.canvas.beginPath();
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(70,95);              // Arrow point
    this.canvas.lineTo(50,75);               // Angle up on right side of arrow
    this.canvas.lineTo(60,75);               // Back in to base of arrow head
    this.canvas.lineTo(60,45);               // Up to bend in arrow
    this.canvas.arc(45, 45, 15, 0, emk::PI, true); // Lower bend
    this.canvas.lineTo(30,95);
    this.canvas.lineTo(10,95);
    this.canvas.lineTo(10,45);
    this.canvas.arc(45, 45, 35, emk::PI, 0, false);
    this.canvas.lineTo(80,75);
    this.canvas.lineTo(90,75);

    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();    
  }
  
  this.button_bar.redoall.DrawIcon = function() {
    // Arrow 1
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(70, 30);
    this.canvas.lineTo(50, 10);
    this.canvas.lineTo(50, 20);
    this.canvas.lineTo(5, 20);
    this.canvas.lineTo(5, 40);
    this.canvas.lineTo(50, 40);
    this.canvas.lineTo(50, 50);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();

    // Arrow 2
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.fillStyle = 'white';
    this.canvas.moveTo(70, 75);
    this.canvas.lineTo(50, 55);
    this.canvas.lineTo(50, 65);
    this.canvas.lineTo(5, 65);
    this.canvas.lineTo(5, 85);
    this.canvas.lineTo(50, 85);
    this.canvas.lineTo(50, 95);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();

    // Wall
    this.canvas.beginPath();
    this.canvas.rect(75, 0, 10, 100);
    this.canvas.fillStyle = '#999';
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
  }
  
  this.button_bar.hint.DrawIcon = function() {
    this.canvas.font = '95pt Helvetica';
    this.canvas.fillStyle = 'green';
    this.canvas.textAlign = 'center';
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fillText('?', 50, 95);
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
  }
  
  this.button_bar.warnings.DrawIcon = function() {
    this.canvas.font = '95pt Helvetica';
    this.canvas.fillStyle = '#800';
    this.canvas.textAlign = 'center';
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fillText('!', 45, 95);
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();
  }
  
  this.button_bar.autonotes.DrawIcon = function() {
    // Board
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.moveTo(10, 0);
    this.canvas.lineTo(10, 100);
    this.canvas.moveTo(90, 0);
    this.canvas.lineTo(90, 100);
    this.canvas.moveTo(0, 10);
    this.canvas.lineTo(100, 10);
    this.canvas.moveTo(0, 90);
    this.canvas.lineTo(100, 90);
    this.canvas.stroke();

    // Pencil marks    
    this.canvas.font = '20pt Helvetica';
    this.canvas.fillStyle = '#008800';
    this.canvas.textAlign = 'center';
    this.canvas.fillText('?', 25, 35);
    this.canvas.fillText('?', 50, 35);
    this.canvas.fillText('?', 75, 35);
    this.canvas.fillText('?', 25, 60);
    this.canvas.fillText('?', 50, 60);
    this.canvas.fillText('?', 75, 60);
    this.canvas.fillText('?', 25, 85);
    this.canvas.fillText('?', 50, 85);
    this.canvas.fillText('?', 75, 85);
    this.canvas.stroke();
  }
  
  this.button_bar.toggleclick.DrawIcon = function() {
    // Board
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.moveTo(10, 0);
    this.canvas.lineTo(10, 100);
    this.canvas.moveTo(90, 0);
    this.canvas.lineTo(90, 100);
    this.canvas.moveTo(0, 10);
    this.canvas.lineTo(100, 10);
    this.canvas.moveTo(0, 90);
    this.canvas.lineTo(100, 90);
    this.canvas.stroke();

    // Pencil marks    
    this.canvas.font = '25pt Helvetica';
    this.canvas.fillStyle = 'blue';
    this.canvas.textAlign = 'center';
    this.canvas.fillText('1', 25, 40);
    this.canvas.fillText('4', 75, 62);
    this.canvas.fillText('7', 25, 85);
    this.canvas.stroke();

    // Pencil
    this.canvas.beginPath();
    this.canvas.lineWidth = 2;
    this.canvas.fillStyle = '#F5DEB3';
    this.canvas.moveTo(30, 85);
    this.canvas.lineTo(30, 70);
    this.canvas.lineTo(60, 0);
    this.canvas.lineTo(70, 5);
    this.canvas.lineTo(40, 75);
    this.canvas.closePath();
    this.canvas.shadowColor = "#999";
    this.canvas.shadowBlur = 4;
    this.canvas.shadowOffsetX = 2;
    this.canvas.shadowOffsetY = 2;
    this.canvas.fill();
    this.canvas.shadowColor = "transparent";
    this.canvas.stroke();    
    this.canvas.beginPath();
    this.canvas.lineWidth = 1;
    this.canvas.moveTo(30,70);
    this.canvas.lineTo(40,75);
    this.canvas.stroke();
  }
  
  
////////////////////
//  ToolTip Layer
////////////////////

  this.tooltip_text = new Kinetic.Text({
    x: 10,
    y: 10,
    text: 'Default Text: This is a test to see how well the tooltips work out.',
    fontSize: 14,
    fontFamily: 'Calibri',
    fill: '#555',
    width: 100,
    padding: 10,
    align: 'center',
    visible: false
  });

  this.tooltip_box = new Kinetic.Rect({
    x: 10,
    y: 10,
    stroke: '#555',
    strokeWidth: 2,
    fill: 'yellow',
    width: 120,
    height: this.tooltip_text.getHeight(),
    shadowColor: 'black',
    shadowBlur: 10,
    shadowOffset: [10, 10],
    shadowOpacity: 0.2,
    cornerRadius: 10,
    visible: false
  });

  this.UpdateTooltip = function(x, y, message) {
    this.tooltip_text.setText(message);
    this.tooltip_box.setHeight(this.tooltip_text.getHeight());
    y = y - this.tooltip_text.getHeight(); // Adjust to be above mouse.
    this.tooltip_text.setX(x+10);
    this.tooltip_text.setY(y);
    this.tooltip_box.setX(x);
    this.tooltip_box.setY(y);    

    this.tooltip_text.show();
    this.tooltip_box.show();
    this.layer_tooltips.draw();
  };
  
  this.HideTooltip = function() {
    this.tooltip_text.hide();
    this.tooltip_box.hide();
    this.layer_tooltips.draw();    
  };


///////////////////////////////////
//  Other interface functionality
///////////////////////////////////

  this.SetState = function(id, new_state)
  {
    // Setup the undo information
    var old_state = this.puzzle.GetState(id);
    var move_info = new cPuzzleMove('value', id, new_state, old_state);
    this.info.undo_stack.push(move_info);

    // Clear the redo stack, since current state information has changed.
    while (this.info.redo_stack.length > 0) {
      this.info.redo_stack.pop();
    }

    // Update buttons that have to do with there being an undo stack.
    this.button_bar.rewind.inactive = false;
    this.button_bar.undo.inactive = false;
    this.button_bar.bookmark.inactive = false;
    this.button_bar.redo.inactive = true;
    this.button_bar.redoall.inactive = true;
    this.layer_buttons.draw();

    // Track with this bookmark.
    var bm_level = this.info.bm_touch[this.info.bm_level];
    bm_level[id]++;

    // Change the actual state of the puzzle.
    this.puzzle.SetState(id, new_state);

    // See if this new state causes changes with warnings or autonotes.
    if (this.info.do_warnings == true) this.UpdateWarnings();
    if (this.info.do_autonotes == true) this.UpdateAutonotes(id, old_state);
  }

  this.SetStateCR = function(col, row, new_state)
  {
    this.SetState( this.puzzle.layout.ToID(col, row), new_state );
  }

  this.ToggleNote = function(id, value, with_last)
  {
    if (with_last === undefined) with_last = false;
  
    // Setup the undo information
    var move_info = new cPuzzleMove('note', id, value);
    if (with_last == false) {
      this.info.undo_stack.push(move_info);
    } else {
      var prev_move = this.info.undo_stack.slice(-1)[0];
      move_info.next = prev_move.next;
      prev_move.next = move_info;
    }

    // Clear the redo stack, since current state information has changed.
    while (this.info.redo_stack.length > 0) {
      this.info.redo_stack.pop();
    }

    // Update buttons that have to do with there being an undo stack.
    this.button_bar.rewind.inactive = false;
    this.button_bar.undo.inactive = false;
    this.button_bar.bookmark.inactive = false;
    this.button_bar.redo.inactive = true;
    this.button_bar.redoall.inactive = true;
    this.layer_buttons.draw();

    // Track with this bookmark.
    var bm_level = this.info.bm_touch[this.info.bm_level];
    bm_level[id]++;

    // And change the actual state of the puzzle.
    this.puzzle.ToggleNote(id, value);
  }
  
  this.SetupBookmarkLevel = function(level) {
    this.info.bm_level = level;
    this.info.bm_touch[level] = [];
    var cur_level = this.info.bm_touch[level];
    for (var i = 0; i < this.puzzle.layout.num_cells; i++) {
      cur_level[i] = 0;
    }
  }
  this.SetupBookmarkLevel(0);

  this.ClearWarnings = function()
  {
    // Mark all cells as not warning player of conflicts.
    for (var i = 0; i < this.puzzle.cell_array.length; i++) {
      this.puzzle.cell_array[i].warn = false;
    }
  }
  this.ClearWarnings(); // Start with no warnings.

  this.UpdateWarnings = function()
  {
    // First clear existing warnings, then determine where they should be.
    this.ClearWarnings();

    // Loop through all regions and flag cells with numbers represented more than once.
    for (var i = 0; i < this.puzzle.layout.region_array.length; i++) {
      var cur_region = this.puzzle.layout.region_array[i];
      var state_count = [];
      var conflict_found = false;
      for (var j = 0; j < cur_region.length; j++) {
        var cur_id = cur_region[j];
        var cur_state = this.puzzle.cell_array[cur_id].state;
        if (state_count[cur_state] === undefined) state_count[cur_state] = 0;
        else {
          state_count[cur_state] = 1;
          conflict_found = true;
        }
      }
      for (var j = 0; conflict_found && j < cur_region.length; j++) {
        var cur_id = cur_region[j];
        var cur_state = this.puzzle.cell_array[cur_id].state;
        if (state_count[cur_state] != 1) continue;
        this.puzzle.cell_array[cur_id].warn = true;
      }
    }
  }
  
  this.UpdateAllAutonotes = function()
  {    
    // Loop through and turn all cells on to begin with.
    for (var i=0; i < this.puzzle.layout.num_cells; i++) {
      if (this.puzzle.cell_array[i].state > 0) continue;  // Don't worry about notes for set cells.
      var cur_reg_ids = this.puzzle.cell_array[i].region_ids;
      for (var test_val = 1; test_val <= 9; test_val++) {
        var found = false;
        for (var j=0; j < cur_reg_ids.length; j++) {
          found |= (this.puzzle.region_track[ cur_reg_ids[j] ][test_val] > 0);
        }
        if (this.puzzle.cell_array[i].notes[test_val] == found) {
          this.ToggleNote(i, test_val, true);          
        }
      }
    }
  }
  
  this.UpdateAutonotes = function(cell_id, old_state)
  {
    if (cell_id === undefined) {
      this.UpdateAllAutonotes();
      return;
    }

    var new_state = this.puzzle.cell_array[cell_id].state;    
    // If this cell's state didn't actually change, stop here.
    if (new_state == old_state) return;
    
    // If we set this cell to a value, block others cells in the same region from that value.
    if (new_state > 0) {
      var reg_ids = this.puzzle.cell_array[cell_id].region_ids; // Grab the region ID's for the current cell.
    
      var alert_count = 0;
    
      // Loop through each cell id in each region
      for (var j=0; j < reg_ids.length; j++) {
        var cur_region = this.puzzle.layout.region_array[reg_ids[j]];
        for (var i = 0; i < cur_region.length; i++) {
          var cur_cell = this.puzzle.cell_array[cur_region[i]];
          if (cur_cell.notes[new_state] == true) {
            // if (alert_count++ < 3) alert('id:' + cur_region[i] + '  new_state:' + new_state);
            this.ToggleNote(cur_region[i], new_state, true);            
          }
        }
      }
    }

  }

  this.DoKeypress = function(cur_key, shift_key)
  {
    if (this.info.toggle_click == true) shift_key = !shift_key;
    if (shift_key == true) cur_key += 1000;
  
    switch (cur_key) {
		case 37:                            // Left Arrow
		case 1037:                          // Shift + Left Arrow
      var cur_col = this.puzzle.layout.ToCol(this.puzzle_board.cell_id);
			if (cur_col > 0) this.puzzle_board.cell_id--;
			this.puzzle_board.getLayer().draw();
			return 1;
		case 38:                            // Up Arrow
		case 1038:                          // Shift + Up Arrow
      var cur_row = this.puzzle.layout.ToRow(this.puzzle_board.cell_id);
      var cur_col = this.puzzle.layout.ToCol(this.puzzle_board.cell_id);
			if (cur_row > 0) this.puzzle_board.cell_id = this.puzzle.layout.ToID(cur_col, cur_row-1);			
			this.puzzle_board.getLayer().draw();
			return 1;
		case 39:                            // Right Arrow
		case 1039:                          // Shift + Right Arrow
      var cur_col = this.puzzle.layout.ToCol(this.puzzle_board.cell_id);
			if (cur_col < 8) this.puzzle_board.cell_id++;
			this.puzzle_board.getLayer().draw();
			return 1;
		case 40:                            // Down Arrow
		case 1040:                          // Shift + Down Arrow
      var cur_row = this.puzzle.layout.ToRow(this.puzzle_board.cell_id);
      var cur_col = this.puzzle.layout.ToCol(this.puzzle_board.cell_id);
			if (cur_row < 8) this.puzzle_board.cell_id = this.puzzle.layout.ToID(cur_col, cur_row+1);
			this.puzzle_board.getLayer().draw();
			return 1;
		case 32:							// Spacebar  -- Clear cell
			cur_key = 48;
		case 48:							// 0         -- Clear cell		
		case 49:                            // 1
		case 50:                            // 2
		case 51:                            // 3
		case 52:                            // 4
		case 53:                            // 5
		case 54:                            // 6
		case 55:                            // 7
		case 56:                            // 8
		case 57:                            // 9
			// Update the current cell if allowed.
			if (this.puzzle.GetLock(this.puzzle_board.cell_id) == 0){
				this.SetState(this.puzzle_board.cell_id, cur_key - 48);
				this.puzzle_board.getLayer().draw();
			}
			return 1;
		case 1049:                          // Shift-1
		case 1050:                          // Shift-2
		case 1051:                          // Shift-3
		case 1052:                          // Shift-4
		case 1053:                          // Shift-5
		case 1054:                          // Shift-6
		case 1055:                          // Shift-7
		case 1056:                          // Shift-8
		case 1057:                          // Shift-9
			// Update the current cell if allowed.
			if (this.puzzle.GetLock(this.puzzle_board.cell_id) == 0){
				this.ToggleNote(this.puzzle_board.cell_id, cur_key - 1048);
				this.puzzle_board.getLayer().draw();
			}
			return 1;
		}

		return 0;
  }

  this.puzzle_board.cell_id = 40;          // Start at middle cell.
  this.puzzle_board.hover_val = 0;         // Which internal value is the mouse over?
  this.puzzle_board.puzzle = this.puzzle;
  this.puzzle_board.interface = this;


//////////////////////////
//  Construct the Layers
//////////////////////////

  // Setup the puzzle layer.
//!
  this.layer_main.add(this.puzzle_board);
//!  this.stage.add(this.layer_main);

  // Setup the button layer
//!  this.layer_buttons = new Kinetic.Layer();
  for (cur_but in this.button_bar) {
    this.layer_buttons.add(this.button_bar[cur_but]);
  }
//!  this.stage.add(this.layer_buttons);

  // Setup the tooltip layer
//!
  this.layer_tooltips.add(this.tooltip_box);
  this.layer_tooltips.add(this.tooltip_text);
//!  this.stage.add(this.layer_tooltips);
}


*/
//////@@@@@@@@@@@@@@@@@



SudokuLayout * my_layout;
SudokuInterface * my_sudoku;

extern "C" int emkMain()
{
  my_layout = new SudokuLayout(9, 9, 9);
  SudokuPuzzle my_puzzle(*my_layout);

  my_puzzle.SetState(0,0, 7);
  my_puzzle.SetState(0,4, 5);
  my_puzzle.SetState(0,6, 8);
  my_puzzle.SetState(0,8, 6);
  my_puzzle.SetState(1,1, 2);
  my_puzzle.SetState(1,4, 9);
  my_puzzle.SetState(2,0, 8);
  my_puzzle.SetState(2,1, 6);
  my_puzzle.SetState(2,5, 3);
  my_puzzle.SetState(2,6, 9);
  my_puzzle.SetState(3,3, 3);
  my_puzzle.SetState(3,5, 2);
  my_puzzle.SetState(3,6, 4);
  my_puzzle.SetState(4,1, 1);
  my_puzzle.SetState(4,3, 5);
  my_puzzle.SetState(4,6, 7);
  my_puzzle.SetState(4,8, 8);
  my_puzzle.SetState(5,2, 6);
  my_puzzle.SetState(5,4, 8);
  my_puzzle.SetState(6,2, 2);
  my_puzzle.SetState(6,4, 3);
  my_puzzle.SetState(6,6, 6);
  my_puzzle.SetState(6,7, 5);
  my_puzzle.SetState(7,1, 5);
  my_puzzle.SetState(7,3, 9);
  my_puzzle.SetState(8,0, 4);
  my_puzzle.SetState(8,3, 6);
  my_puzzle.SetState(8,6, 1);
  my_puzzle.SetState(8,8, 2);
  
  my_puzzle.LockAllSet();
        
  my_sudoku = new SudokuInterface(my_puzzle);

  return 0;
}
