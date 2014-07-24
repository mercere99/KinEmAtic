#include <emscripten.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>
#include <stdlib.h>
#include <vector>

#include "../libs/Kinetic.h"

#include "../tools/const.h"
#include "../tools/functions.h"
#include "../cogs/Button.h"

class SudokuLayout {
private:
  const int grid_x;
  const int grid_y;
  const int num_symbols;
  const int num_cells;
  std::vector<int *> region_array;
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
  std::vector<bool> notes;
  bool warn;
  bool lock;
  std::vector<int> region_ids;
public:
  SudokuCell() : state(0), notes(10), lock(false) {
    for (int i = 1; i <= 9; i++) notes[i] = false;
  }
  SudokuCell(const SudokuCell & _in) : state(_in.state), notes(_in.notes), lock(_in.lock), region_ids(_in.region_ids)
  { ; }
  ~SudokuCell() { ; }
  
  int GetState() const { return state; }
  const std::vector<bool> & GetNotes() { return notes; }
  bool GetNote(int id) const { return notes[id]; }
  bool GetWarn() const { return warn; }
  bool GetLock() const { return lock; }
  int GetNumRegions() const { return (int) region_ids.size(); }
  const std::vector<int> & GetRegionIDs() { return region_ids; }
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
  std::vector<SudokuCell> cell_array;      // Info about the state of each cell in the puzzle
  std::vector<std::vector<int> > region_track; // Which values are set in each region?

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

  void SetWarn(int cell_id, bool setting) { cell_array[cell_id].SetWarn(setting); }
};



/////////////////////////////
//  Track move information
/////////////////////////////

class PuzzleMove {
public:
  enum Type { VALUE, NOTE, AUTONOTES, BOOKMARK };

  Type type;      // What type of move was this?
  int id;         // ID of cell affected.
  int new_state;  // New state of affected cell, if relevant.
  int prev_state; // Previous state of affected cell, if relevant.
  PuzzleMove * next;      // Is this move clustered with others for a single undo?

public:
  PuzzleMove(Type _type, int _id, int _new_state, int _prev_state)
    : type(_type), id(_id), new_state(_new_state), prev_state(_prev_state), next(NULL) { ; }
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

  // Graphical elements
  emk::CustomShape puzzle_board; // @CAO Should this be an emkRect to capture mouse clicks?
  emk::Button button_rewind;
  emk::Button button_undo;
  emk::Button button_bookmark;
  emk::Button button_redo;
  emk::Button button_redoall;
  emk::Button button_hint;
  emk::ToggleButton button_warnings;
  emk::ToggleButton button_autonotes;
  emk::ToggleButton button_toggleclick;
  emk::Layer layer_main;
  emk::Layer layer_buttons;
  emk::Layer layer_tooltips;
  emk::Stage stage;

  // Current state
  int cell_id;                   // Current cell in focus
  int highlight_id;              // Is there a cell we should be highlighting?
  int hover_val;                 // The note value being hovered over...
  int bm_level;                  // Number of bookmarks set so far.
  std::vector<std::vector<int> > bm_touch; // How many times has each cell been touched in each bookmark level?
  bool do_warnings;              // Should we warn when there is an obvious error?
  bool do_autonotes;             // Should we automatically provide notes?
  bool toggle_click;             // Should clicks be for autonotes by default?
    
  std::stack<PuzzleMove *> undo_stack;
  std::stack<PuzzleMove *> redo_stack;

public:
  SudokuInterface(const SudokuPuzzle & _puzzle)
    : puzzle(_puzzle)
    , min_size(500), cell_padding(5), border(8), mid_width(4), thin_width(2)
    , puzzle_board(this, &SudokuInterface::DrawGrid)
      
    , button_rewind(this, &SudokuInterface::DoRewind, "Rewind")
    , button_undo(this, &SudokuInterface::DoUndo, "Undo")
    , button_bookmark(this, &SudokuInterface::DoBookmark, "Bookmark")
    , button_redo(this, &SudokuInterface::DoRedo, "Redo")
    , button_redoall(this, &SudokuInterface::DoRedoall, "Redoall")
    , button_hint(this, &SudokuInterface::DoHint, "Hint")
    , button_warnings(this, &SudokuInterface::DoWarnings, "Warnings")
    , button_autonotes(this, &SudokuInterface::DoAutonotes, "Autonotes")
    , button_toggleclick(this, &SudokuInterface::DoToggleclick, "Toggleclick")

    , stage(1200, 600, "container")
    , cell_id(0), highlight_id(-1), hover_val(-1), bm_level(-1), bm_touch(0)
    , do_warnings(false), do_autonotes(false), toggle_click(false)
  {
    IncBookmarkLevel();
    ClearWarnings();
    stage.ResizeMax(min_size, min_size);
    ResizeBoard();

    // Setup rounded corners on buttons... Layout: { | | | | } { } { | | }
    button_rewind.SetRoundCorners(true, false, false, true);
    button_undo.SetRoundCorners(false, false, false, false);
    button_bookmark.SetRoundCorners(false, false, false, false);
    button_redo.SetRoundCorners(false, false, false, false);
    button_redoall.SetRoundCorners(false, true, true, false);
    button_hint.SetRoundCorners(true, true, true, true);
    button_warnings.SetRoundCorners(true, false, false, true);
    button_autonotes.SetRoundCorners(false, false, false, false);
    button_toggleclick.SetRoundCorners(false, true, true, false);

    // Some buttons start out inactive.
    button_rewind.SetActive(false);
    button_undo.SetActive(false);
    button_bookmark.SetActive(false);
    button_redo.SetActive(false);
    button_redoall.SetActive(false);

    // Setup button tooltips
    button_rewind.SetToolTip("Rewind to bookmark");
    button_undo.SetToolTip("Undo");
    button_bookmark.SetToolTip("Add bookmark");
    button_redo.SetToolTip("Redo");
    button_redoall.SetToolTip("Redo ALL");
    button_hint.SetToolTip("Get HINT");
    button_warnings.SetToolTip("Toggle highlighting of conflicts");
    button_autonotes.SetToolTip("Toggle automatic pencilmarks");
    button_toggleclick.SetToolTip("Toggle answers & pencilmarks");

    // Setup button icons
    button_rewind.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Rewind);
    button_undo.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Undo);
    button_bookmark.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Bookmark);
    button_redo.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Redo);
    button_redoall.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Redoall);
    button_hint.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Hint);
    button_warnings.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Warnings);
    button_autonotes.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Autonotes);
    button_toggleclick.SetupDrawIcon(this, &SudokuInterface::DrawIcon_Toggleclick);

    // puzzle_board.On("click", this, &SudokuInterface::OnClick);
    puzzle_board.On("mousedown", this, &SudokuInterface::OnMousedown);
    puzzle_board.On("mousemove", this, &SudokuInterface::OnMousemove);

    layer_main.Add(puzzle_board);
    layer_buttons.Add(button_rewind);
    layer_buttons.Add(button_undo);
    layer_buttons.Add(button_bookmark);
    layer_buttons.Add(button_redo);
    layer_buttons.Add(button_redoall);
    layer_buttons.Add(button_hint);
    layer_buttons.Add(button_warnings);
    layer_buttons.Add(button_autonotes);
    layer_buttons.Add(button_toggleclick);
    stage.Add(layer_main);
    stage.Add(layer_buttons);
    stage.Add(layer_tooltips);
  }
  ~SudokuInterface() { ; }

  // void OnClick(const emk::EventInfo & info) { emk::Alert(info.shift_key); }

  void OnMousedown(const emk::EventInfo & info)
  {
    // If this cell is locked, stop here -- there's nothing to do.
    if (puzzle.GetLock(cell_id) == true) return;

    // If this cell was previously set, clear its current state.
    if (puzzle.GetState(cell_id) > 0) puzzle.SetState(cell_id, 0);      

    // Otherwise, see which value is being clicked on and set the state.
    else {
      // Shift indicates that pencil-marks should be done instead of setting the value.
      if (info.shift_key ^ toggle_click) {
        puzzle.ToggleNote(cell_id, hover_val);
      } else {
        puzzle.SetState(cell_id, hover_val);
      }
    }
    
    layer_main.Draw();
  }

  void OnMousemove(const emk::EventInfo & info)
  {
    const double rel_x = ((double) (info.layer_x - border)) / (double) cell_width;
    const double rel_y = ((double) (info.layer_y - border)) / (double) cell_width;
    const int cur_col = rel_x;
    const int cur_row = rel_y;

    cell_id = puzzle.GetLayout().ToID(cur_col, cur_row);
    
    double cell_x = rel_x - (double) cur_col;
    double cell_y = rel_y - (double) cur_row;

    hover_val = ((int) (cell_x * 3.0)) + 3 * ((int) (cell_y * 3.0)) + 1;

    layer_main.Draw();
  }


  void ResizeBoard() {
    int win_size = std::min((int)(stage.GetWidth() * 1.25), stage.GetHeight()); // Use the window size.
    win_size = std::max(win_size, min_size); // However, don't let puzzle get too small.
    puzzle_size = win_size * 0.8;

    // These are dynamically sized numbers.
    cell_width = (puzzle_size - border*2)/9;
    region_width = cell_width * 3;
    board_width = cell_width * 9;

    puz_text_pt = cell_width - 2 * cell_padding;     // Font size for text filling a cell.
    puz_note_width = cell_width / 3 - 1;             // Space for note characters in each cell
    puz_note_pt = puz_note_width - 1;                // Font size for smaller characters
    full_width = board_width + border*2;

    button_w = full_width/10;     // Width of each button
    const int x_offset = 3;
    const int y_offset = puzzle_size + 2;

    button_rewind.SetLayout(x_offset, y_offset, button_w, button_w);
    button_undo.SetLayout(x_offset + button_w, y_offset, button_w, button_w);
    button_bookmark.SetLayout(x_offset + button_w*2, y_offset, button_w, button_w);
    button_redo.SetLayout(x_offset + button_w*3, y_offset, button_w, button_w);
    button_redoall.SetLayout(x_offset + button_w*4, y_offset, button_w, button_w);
    button_hint.SetLayout(button_w*5.5, y_offset, button_w, button_w);
    button_warnings.SetLayout(button_w*7 - x_offset, y_offset, button_w, button_w);
    button_autonotes.SetLayout(button_w*8 - x_offset, y_offset, button_w, button_w);
    button_toggleclick.SetLayout(button_w*9 - x_offset, y_offset, button_w, button_w);
  }

  void DrawGrid(emk::Canvas & canvas) {
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
    const std::vector<int> & bm_touch_cur = bm_touch[bm_level];

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
              canvas.Text(std::to_string(val), text_x, text_y);
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
        canvas.SetFont(std::to_string(puz_note_pt) + "pt Calibri");
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


////////////////////////////////////
//  Button Callbacks
////////////////////////////////////

  void DoRewind() {
    int step = 0;
    PuzzleMove * last_move;
    while (undo_stack.size() > 0) {
      if (undo_stack.top()->type == PuzzleMove::BOOKMARK) break;
      last_move = undo_stack.top();

      emk::ScheduleMethod(this, &SudokuInterface::DoUndo, 100*step);
      step++;
    }
    highlight_id = last_move->id;
    // Flash the first cell changed after laying the bookmark.
    emk::ScheduleMethod(this, &SudokuInterface::DoRedo, 100*step + 200);
    emk::ScheduleMethod(this, &SudokuInterface::DoUndo, 100*step + 400);
    emk::ScheduleMethod(this, &SudokuInterface::DoRedo, 100*step + 600);
    emk::ScheduleMethod(this, &SudokuInterface::DoUndo, 100*step + 800);

    // One extra undo to clear the bookmark.
    emk::ScheduleMethod(this, &SudokuInterface::DoUndo, 100*step + 1000); 
  }


  void DoUndo() {
    // Grab the last action done.
    PuzzleMove * undo_item = undo_stack.top();

    // Move it to the redo stack
    undo_stack.pop();
    redo_stack.push(undo_item);

    // Undo each associated action.
    while (undo_item != NULL) {
      // Reverse this item.
      if (undo_item->type == PuzzleMove::VALUE) {
        puzzle.SetState(undo_item->id, undo_item->prev_state);        
        bm_touch[bm_level][undo_item->id]--;                  // Update bookmark tracking...
      } else if (undo_item->type == PuzzleMove::NOTE) {
        puzzle.ToggleNote(undo_item->id, undo_item->new_state);
      } else if (undo_item->type == PuzzleMove::BOOKMARK) {
        bm_level--;
      } else if (undo_item->type == PuzzleMove::AUTONOTES) {
        do_autonotes = !do_autonotes;
        button_autonotes.Toggle();
      } 

      undo_item = undo_item->next;
    }
    
    // Update buttons that have to do with the undo and redo stacks.
    if (undo_stack.size() == 0) {      // If the undo stack is empty, gray-out related buttons
      button_rewind.SetActive(false);
      button_undo.SetActive(false);
      button_bookmark.SetActive(false);
    }
    else if (undo_stack.top()->type == PuzzleMove::BOOKMARK) {
      button_bookmark.SetActive(false);
    }
    else {
      button_bookmark.SetActive(true);
    }

    button_redo.SetActive(false);
    button_redoall.SetActive(false);
    
    // Update Warnings
    if (do_warnings == true) UpdateWarnings();

    // And redraw everything that's changed.
    layer_main.Draw();
    layer_buttons.Draw();
  }

  void DoBookmark() {
    IncBookmarkLevel();

    // Put this move into the undo stack.
    undo_stack.push(new PuzzleMove(PuzzleMove::BOOKMARK, -1, -1, -1));

    // Gray-out the bookmark
    button_bookmark.SetActive(false);
    
    layer_main.Draw();
  }

  void DoRedo() {
    if (redo_stack.size() == 0) return;
    
    // Find the next item to redo
    PuzzleMove * redo_item = redo_stack.top();

    // Move it back into the undo stack.
    redo_stack.pop();
    undo_stack.push(redo_item);

    // Redo each item in this group.
    while (redo_item != NULL) {
      if (redo_item->type == PuzzleMove::VALUE) {
        puzzle.SetState(redo_item->id, redo_item->new_state);
        bm_touch[bm_level][redo_item->id]++;
      } else if (redo_item->type == PuzzleMove::NOTE) {
        puzzle.ToggleNote(redo_item->id, redo_item->new_state);      
      } else if (redo_item->type == PuzzleMove::BOOKMARK) {
        IncBookmarkLevel();
        button_bookmark.SetActive(false);
      } else if (redo_item->type == PuzzleMove::AUTONOTES) {
        do_autonotes = !do_autonotes;
        button_autonotes.Toggle();
      }

      redo_item = redo_item->next;
    }
        

    // Update buttons that have to do with the undo and redo stacks.
    button_rewind.SetActive(true);
    button_undo.SetActive(true);
    button_bookmark.SetActive(true);
    if (redo_stack.size() == 0) {
      button_redo.SetActive(false);
      button_redoall.SetActive(false);
    }
    
    // Update Warnings
    if (do_warnings == true) UpdateWarnings();

    // And redraw everything that's changed.
    layer_main.Draw();
    layer_buttons.Draw();
  }

  void DoRedoall() {
    for (int i=0; i < (int) redo_stack.size(); i++) {
      emk::ScheduleMethod(this, &SudokuInterface::DoRedo, 100*i); 
    }
  }

  void DoHint() {
    // @CAO Write this!
  }

  void DoWarnings() {
    do_warnings = !do_warnings;
    if (do_warnings) UpdateWarnings();
    else ClearWarnings();
    layer_main.Draw();
  }

  void DoAutonotes() {
    PuzzleMove * move_info = new PuzzleMove(PuzzleMove::AUTONOTES, -1, -1, -1);
    undo_stack.push(move_info);
    do_autonotes = !do_autonotes;
    if (do_autonotes) UpdateAllAutonotes();
    layer_main.Draw();
  }

  void DoToggleclick() {
    toggle_click = !toggle_click;
  }
  

///////////////////////
//  Button Finishing!
///////////////////////

  void UpdateHover_Undo() {
    if (undo_stack.size() == 0) {
      highlight_id = -1;
      button_undo.SetToolTip("Nothing to UNDO");
      return;
    }
    PuzzleMove * stack_top = undo_stack.top();
    if (stack_top->type == PuzzleMove::VALUE) {
      highlight_id = stack_top->id;
      button_undo.SetToolTip(std::string("UNDO value = ") + std::to_string(stack_top->new_state));
    }
    if (stack_top->type == PuzzleMove::NOTE) {
      highlight_id = stack_top->id;
      button_undo.SetToolTip(std::string("UNDO pencilmark toggle ") + std::to_string(stack_top->new_state));
    }
    if (stack_top->type == PuzzleMove::BOOKMARK) {
      button_undo.SetToolTip("UNDO bookmark");
      highlight_id = -1;
    }
    if (stack_top->type == PuzzleMove::AUTONOTES) {
      button_undo.SetToolTip("UNDO pencil marks");
      highlight_id = -1;
    }
  }

  void UpdateHover_Bookmark() {
    if (undo_stack.size() == 0) {
      button_bookmark.SetToolTip("No BOOKMARK needed at start");
      return;
    }  
    if (undo_stack.top()->type == PuzzleMove::BOOKMARK) {      
      button_bookmark.SetToolTip("Already at BOOKMARK");
      return;
    }
    button_bookmark.SetToolTip("Add BOOKMARK");
  }
  
  void UpdateHover_Redo() {
    if (redo_stack.size() == 0) {
      highlight_id = -1;
      button_redo.SetToolTip("Nothing to REDO");
      return;
    }
    PuzzleMove * stack_top = redo_stack.top();
    if (stack_top->type == PuzzleMove::VALUE) {
      highlight_id = stack_top->id;
      button_redo.SetToolTip(std::string("REDO value = ") + std::to_string(stack_top->new_state));
    }
    if (stack_top->type == PuzzleMove::NOTE) {
      highlight_id = stack_top->id;
      button_redo.SetToolTip(std::string("REDO pencilmark toggle ") + std::to_string(stack_top->new_state));
    }
    if (stack_top->type == PuzzleMove::BOOKMARK) {
      button_redo.SetToolTip("REDO bookmark");
      highlight_id = -1;
    }
    if (stack_top->type == PuzzleMove::AUTONOTES) {
      button_redo.SetToolTip("REDO pencil marks");
      highlight_id = -1;
    }
  }

  // Setup icons
  // Note: All icons are adjusted to be on a 100x100 grid.
  
  void DrawIcon_Rewind(emk::Canvas & canvas) {
    // Draw the bookmark
    canvas.BeginPath();
    canvas.SetLineWidth(3);
    canvas.SetFillStyle("#F5DEB3");
    canvas.MoveTo(5, 100);
    canvas.Arc(15, 10, 10, emk::PI, emk::PI*1.5);
    canvas.Arc(35, 10, 10, emk::PI*1.5, 0);
    canvas.LineTo(45, 100);
    canvas.LineTo(25, 90);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(3);
    canvas.SetShadowOffsetY(3);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();

    // Draw Arrow 1
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.SetFillStyle("white");
    canvas.MoveTo(45, 30);
    canvas.LineTo(65, 10);
    canvas.LineTo(65, 20);
    canvas.LineTo(95, 20);
    canvas.LineTo(95, 40);
    canvas.LineTo(65, 40);
    canvas.LineTo(65, 50);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(3);
    canvas.SetShadowOffsetY(3);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
    
    // Draw Arrow 2  
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.SetFillStyle("white");
    canvas.MoveTo(45, 75);
    canvas.LineTo(65, 55);
    canvas.LineTo(65, 65);
    canvas.LineTo(95, 65);
    canvas.LineTo(95, 85);
    canvas.LineTo(65, 85);
    canvas.LineTo(65, 95);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(3);
    canvas.SetShadowOffsetY(3);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
  }

  void DrawIcon_Undo(emk::Canvas & canvas) {
    canvas.BeginPath();
    canvas.SetFillStyle("white");
    canvas.MoveTo(30,95);              // Arrow point
    canvas.LineTo(50,75);               // Angle up on right side of arrow
    canvas.LineTo(40,75);               // Back in to base of arrow head
    canvas.LineTo(40,45);               // Up to bend in arrow
    canvas.Arc(55, 45, 15, emk::PI, 0); // Lower bend
    canvas.LineTo(70,95);
    canvas.LineTo(90,95);
    canvas.LineTo(90,45);
    canvas.Arc(55, 45, 35, 0, emk::PI, true);
    canvas.LineTo(20,75);
    canvas.LineTo(10,75);

    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();    
  }
  

  void DrawIcon_Bookmark(emk::Canvas & canvas) {
    // Grid
    const int gridx = 5;
    const int gridy = 10;
    const int gridw = 90;
    const int gridh = 90;
    const int grid_lines = 6;
    const int grid_step = gridw / (grid_lines - 1);

    const int gridx2 = gridx + gridw;
    const int gridy2 = gridy + gridh;
    
    canvas.BeginPath();
    canvas.SetLineWidth(3);
    canvas.SetFillStyle("white");
    canvas.Rect(gridx, gridy, gridw, gridh);
    canvas.Fill();
    for (int i = 0; i < grid_lines; i++) {
	    canvas.MoveTo(gridx,             gridy+grid_step*i);
	    canvas.LineTo(gridx2,            gridy+grid_step*i);
	    canvas.MoveTo(gridx+grid_step*i, gridy);
	    canvas.LineTo(gridx+grid_step*i, gridy2);
    }
    canvas.Stroke();    

    canvas.BeginPath();
    canvas.SetLineWidth(3);
    canvas.SetFillStyle("#F5DEB3");
    canvas.MoveTo(15, 100);
    canvas.Arc(25, 10, 10, emk::PI, emk::PI*1.5);
    canvas.Arc(45, 10, 10, emk::PI*1.5, 0);
    canvas.LineTo(55, 100);
    canvas.LineTo(35, 90);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(3);
    canvas.SetShadowOffsetY(3);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
  }

  void DrawIcon_Redo(emk::Canvas & canvas) {
    canvas.BeginPath();
    canvas.SetFillStyle("white");
    canvas.MoveTo(70,95);              // Arrow point
    canvas.LineTo(50,75);               // Angle up on right side of arrow
    canvas.LineTo(60,75);               // Back in to base of arrow head
    canvas.LineTo(60,45);               // Up to bend in arrow
    canvas.Arc(45, 45, 15, 0, emk::PI, true); // Lower bend
    canvas.LineTo(30,95);
    canvas.LineTo(10,95);
    canvas.LineTo(10,45);
    canvas.Arc(45, 45, 35, emk::PI, 0, false);
    canvas.LineTo(80,75);
    canvas.LineTo(90,75);

    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();    
  }
  
  void DrawIcon_Redoall(emk::Canvas & canvas) {
    // Arrow 1
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.SetFillStyle("white");
    canvas.MoveTo(70, 30);
    canvas.LineTo(50, 10);
    canvas.LineTo(50, 20);
    canvas.LineTo(5, 20);
    canvas.LineTo(5, 40);
    canvas.LineTo(50, 40);
    canvas.LineTo(50, 50);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();

    // Arrow 2
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.SetFillStyle("white");
    canvas.MoveTo(70, 75);
    canvas.LineTo(50, 55);
    canvas.LineTo(50, 65);
    canvas.LineTo(5, 65);
    canvas.LineTo(5, 85);
    canvas.LineTo(50, 85);
    canvas.LineTo(50, 95);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();

    // Wall
    canvas.BeginPath();
    canvas.Rect(75, 0, 10, 100);
    canvas.SetFillStyle("#999");
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
  }
  
  void DrawIcon_Hint(emk::Canvas & canvas) {
    canvas.SetFont("95pt Helvetica");
    canvas.SetFillStyle("green");
    canvas.SetTextAlign("center");
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Text("?", 50, 95);
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
  }
  
  void DrawIcon_Warnings(emk::Canvas & canvas) {
    canvas.SetFont("95pt Helvetica");
    canvas.SetFillStyle("#800");
    canvas.SetTextAlign("center");
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Text("!", 45, 95);
    canvas.SetShadowColor("transparent");
    canvas.Stroke();
  }
  
  void DrawIcon_Autonotes(emk::Canvas & canvas) {
    // Board
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.MoveTo(10, 0);
    canvas.LineTo(10, 100);
    canvas.MoveTo(90, 0);
    canvas.LineTo(90, 100);
    canvas.MoveTo(0, 10);
    canvas.LineTo(100, 10);
    canvas.MoveTo(0, 90);
    canvas.LineTo(100, 90);
    canvas.Stroke();

    // Pencil marks    
    canvas.SetFont("20pt Helvetica");
    canvas.SetFillStyle("#008800");
    canvas.SetTextAlign("center");
    canvas.Text("?", 25, 35);
    canvas.Text("?", 50, 35);
    canvas.Text("?", 75, 35);
    canvas.Text("?", 25, 60);
    canvas.Text("?", 50, 60);
    canvas.Text("?", 75, 60);
    canvas.Text("?", 25, 85);
    canvas.Text("?", 50, 85);
    canvas.Text("?", 75, 85);
    canvas.Stroke();
  }
  
  void DrawIcon_Toggleclick(emk::Canvas & canvas) {
    // Board
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.MoveTo(10, 0);
    canvas.LineTo(10, 100);
    canvas.MoveTo(90, 0);
    canvas.LineTo(90, 100);
    canvas.MoveTo(0, 10);
    canvas.LineTo(100, 10);
    canvas.MoveTo(0, 90);
    canvas.LineTo(100, 90);
    canvas.Stroke();

    // Pencil marks    
    canvas.SetFont("25pt Helvetica");
    canvas.SetFillStyle("blue");
    canvas.SetTextAlign("center");
    canvas.Text("1", 25, 40);
    canvas.Text("4", 75, 62);
    canvas.Text("7", 25, 85);
    canvas.Stroke();

    // Pencil
    canvas.BeginPath();
    canvas.SetLineWidth(2);
    canvas.SetFillStyle("#F5DEB3");
    canvas.MoveTo(30, 85);
    canvas.LineTo(30, 70);
    canvas.LineTo(60, 0);
    canvas.LineTo(70, 5);
    canvas.LineTo(40, 75);
    canvas.ClosePath();
    canvas.SetShadowColor("#999");
    canvas.SetShadowBlur(4);
    canvas.SetShadowOffsetX(2);
    canvas.SetShadowOffsetY(2);
    canvas.Fill();
    canvas.SetShadowColor("transparent");
    canvas.Stroke();    
    canvas.BeginPath();
    canvas.SetLineWidth(1);
    canvas.MoveTo(30,70);
    canvas.LineTo(40,75);
    canvas.Stroke();
  }
  
  /*
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
    var move_info = new cPuzzleMove(PuzzleMove::VALUE, id, new_state, old_state);
    undo_stack.push(move_info);

    // Clear the redo stack, since current state information has changed.
    while (redo_stack.size() > 0) {
      redo_stack.pop();
    }

    // Update buttons that have to do with there being an undo stack.
    button_rewind.inactive = false;
    button_undo.inactive = false;
    button_bookmark.inactive = false;
    button_redo.SetActive(false);
    button_redoall.SetActive(false);
    layer_buttons.Draw();

    // Track with this bookmark.
    var bm_level = bm_touch[bm_level];
    bm_level[id]++;

    // Change the actual state of the puzzle.
    this.puzzle.SetState(id, new_state);

    // See if this new state causes changes with warnings or autonotes.
    if (do_warnings == true) this.UpdateWarnings();
    if (do_autonotes == true) this.UpdateAutonotes(id, old_state);
  }

  this.SetStateCR = function(col, row, new_state)
  {
    this.SetState( this.puzzle.layout.ToID(col, row), new_state );
  }

  this.ToggleNote = function(id, value, with_last)
  {
    if (with_last === undefined) with_last = false;
  
    // Setup the undo information
    var move_info = new cPuzzleMove(PuzzleMove::NOTE, id, value);
    if (with_last == false) {
      undo_stack.push(move_info);
    } else {
      var prev_move = undo_stack.top();
      move_info.next = prev_move.next;
      prev_move.next = move_info;
    }

    // Clear the redo stack, since current state information has changed.
    while (redo_stack.size() > 0) {
      redo_stack.pop();
    }

    // Update buttons that have to do with there being an undo stack.
    button_rewind.inactive = false;
    button_undo.inactive = false;
    button_bookmark.inactive = false;
    button_redo.SetActive(false);
    button_redoall.SetActive(false);
    layer_buttons.Draw();

    // Track with this bookmark.
    var bm_level = bm_touch[bm_level];
    bm_level[id]++;

    // And change the actual state of the puzzle.
    this.puzzle.ToggleNote(id, value);
  }
  
*/

  void IncBookmarkLevel() {
    if (++bm_level >= (int) bm_touch.size()) {
      bm_touch.resize(bm_level+1);
      bm_touch[bm_level].resize(81);
    }
    for (int i = 0; i < 81; i++) bm_touch[bm_level][i] = 0;
  }


  void ClearWarnings() {  // Mark all cells as not warning player of conflicts.
    for (int i = 0; i < 81; i++) puzzle.SetWarn(i, false);
  }


  void UpdateWarnings()
  {
    /*
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
    */
  }
  
  void UpdateAllAutonotes()
  {    
/*
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
*/
  }


  void UpdateAutonotes(int cell_id, int old_state)
  {
    /*
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
    */
  }
};

/*
  this.DoKeypress = function(cur_key, shift_key)
  {
    if (toggle_click == true) shift_key = !shift_key;
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
