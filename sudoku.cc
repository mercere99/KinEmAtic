#include <emscripten.h>

#include <vector>
#include <iostream>
#include <cmath>

#include "Kinetic.h"

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
  int toRow(int id) const { return id / grid_x; }
};


class SudokuCell {
private:
  int state;
  vector<bool> notes;
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
  bool GetLock() const { return lock; }
  int GetNumRegions() const { return (int) region_ids.size(); }
  const vector<int> & GetRegionIDs() { return region_ids; }
  int GetRegionID(int pos) { return region_ids[pos]; }
  
  void SetState(int _state) { state = _state; }
  void SetNote(int note_id, bool setting=true) { notes[note_id] = setting; }
  void ToggleNote(int note_id) { notes[note_id] = !notes[note_id]; }
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


  int GetState(int cell_id) const { return cell_array[cell_id].GetState(); }
  int GetState(int col, int row) const { return GetState(layout.ToID(col, row)); }
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
  int puzzle_size;   // Default xy pixels for puzzle
  int min_size;      // Minimum puzzle size with resizing
  int cell_padding;  // Pedding between edge of cell and internal number.
  int border;        // Thickness of outer border.
  int mid_width;     // Thickness of border between 3x3 square regions.
  int thin_width;    // Thickness of normal cell walls.

  // Calculated info
  int cell_width;    // Width (and height) of an individual grid cell.
  int region_width;  // Width (and height) of a 3x3 region
  int board_width;   // Width (and height) of the entire board

  // Graphical elements
  emkStage stage;
  emkLayer layer_main;
  emkLayer layer_buttons;

  emkCustomShape test_shape;

public:
  SudokuInterface(const SudokuPuzzle & _puzzle)
    : puzzle(_puzzle)
    , puzzle_size(500), min_size(500), cell_padding(5), border(8), mid_width(4), thin_width(2)
    , cell_width(0), region_width(0), board_width(0) // @CAO Cut line since these get calculate later?
    , stage(1200, 600, "container")
    , test_shape(100, 100, this, &SudokuInterface::DrawGrid)
  {
    stage.ResizeMax(min_size, min_size);

    /////// TEST //////
    layer_main.Add(test_shape);
    stage.Add(layer_main);
  }
  ~SudokuInterface() { ; }

  void DrawGrid(emkCanvas & canvas) {
    canvas.Rect(10,10,50,50,true);
    canvas.Draw();
  }
};


/////@@@@@@@@@@@@@@@@@@
/*

  // Track the basic puzzle information.
  this.info = {

    // Button sizing
    buttons_x: {},    // Map of button x-values
    buttons_y: 0,     // Y-value of main button bar
    buttons_w: 0,     // Width of each button
    buttons_h: 0,     // Height of each button
    buttons_r: 10,    // Corner radius of end-buttons
 
    // Undo/redo stacks
    undo_stack: [],
    redo_stack: [],
    
    // Bookmark info.
    bm_level: 0,      // Number of bookmarks set so far.
    bm_touch: [],     // Has each cell been touched in the current bookmark level?
    
    // Toggle button status
    do_warnings: false,
    do_autonotes: false,
    toggle_click: false,
    
    // Highlight info
    highlight_id: -1,
  };
  this.info.interface = this; // Let the info point back to the interface.


  // &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


  // Build the object to manage the visual board.
  this.puzzle_board = new Kinetic.Shape({
    drawFunc: function(canvas) {
      var ctx = canvas.getContext();
      var offset = this.info.border;

      var active_col = this.puzzle.layout.ToCol(this.cell_id);
      var active_row = this.puzzle.layout.ToRow(this.cell_id);
      var cell_x = offset + this.info.cell_width * active_col;
      var cell_y = offset + this.info.cell_width * active_row;
  
      // Clear out the background
      ctx.fillStyle = "rgb(255,250,245)";
      ctx.fillRect(offset, offset, this.info.board_width, this.info.board_width);

      // Highlight the regions affected.
      if (active_row != -1 && active_col != -1) {
        ctx.fillStyle = "rgba(200, 200, 250, 0.5)";  // Highlight color
//        ctx.fillStyle = "rgba(255, 255, 200, 0.9)";  // Col (yellow)
        ctx.fillRect(cell_x, offset, this.info.cell_width, this.info.board_width)

//        ctx.fillStyle = "rgba(255, 220, 220, 0.5)";     // Row (red)
        ctx.fillRect(offset, cell_y, this.info.board_width, this.info.cell_width)
 	 
//        ctx.fillStyle = "rgba(200, 200, 255, 0.5)";    // Square Region (blue)
        var region_x = Math.floor(active_col / 3);
        var region_y = Math.floor(active_row / 3);
        ctx.fillRect(offset + region_x * this.info.region_width, offset + region_y * this.info.region_width,
                     this.info.region_width, this.info.region_width)

        ctx.fillStyle = "rgb(250, 250, 250)";     // Target (white)
        ctx.fillRect(cell_x, cell_y, this.info.cell_width, this.info.cell_width);
        
        // If there is a cell to highlight, do so.
        if (this.info.highlight_id != -1) {
          var h_col = this.puzzle.layout.ToCol(this.info.highlight_id);
          var h_row = this.puzzle.layout.ToRow(this.info.highlight_id);
          var hx = offset + this.info.cell_width * h_col;
          var hy = offset + this.info.cell_width * h_row;
          ctx.fillStyle = "yellow";
          ctx.fillRect(hx, hy, this.info.cell_width, this.info.cell_width);
        }
      }
  
      // Setup drawing the actual grid.
      ctx.beginPath();
      ctx.fillStyle = "black";
      ctx.lineJoin = "round";
      ctx.lineWidth = this.info.mid_width;
      
      // Draw region borders
      ctx.strokeRect(offset + this.info.region_width, offset/2, this.info.region_width, this.info.board_width+offset);
      ctx.strokeRect(offset/2, offset + this.info.region_width, this.info.board_width+offset, this.info.region_width);

      // Draw thin lines
      ctx.lineWidth = this.info.thin_width;
      for (var i = 1; i < 9; i++) {
	      x = i*this.info.cell_width + offset;
	      y = i*this.info.cell_width + offset;
	      ctx.moveTo(x, offset);
	      ctx.lineTo(x, offset + this.info.board_width);	  
	      ctx.moveTo(offset, y);
	      ctx.lineTo(offset + this.info.board_width, y);	  
	    }
	    ctx.stroke();

	    // Outer box
	    ctx.lineWidth = this.info.border;
	    
	    var corner_radius = 10;
	    var dist1 = offset/2 + corner_radius;
	    var dist2 = this.info.board_width + 3 * offset/2 - corner_radius;

	    ctx.beginPath();
	    ctx.moveTo(dist1 - corner_radius, dist1);
	    ctx.arc(dist1, dist1, corner_radius, Math.PI, 3*Math.PI/2, false);
	    ctx.arc(dist2, dist1, corner_radius, 3*Math.PI/2, 0, false);
	    ctx.arc(dist2, dist2, corner_radius, 0, Math.PI/2, false);
	    ctx.arc(dist1, dist2, corner_radius, Math.PI/2, Math.PI, false);
	    ctx.closePath();
	    ctx.stroke();

	    ctx.beginPath();

	    // Setup this bookmark level for coloring.
	    var bm_level = this.info.bm_touch[this.info.bm_level];

	    // Fill in the values
	    ctx.textAlign = 'center';
	    for (var i = 0; i < 9; i++) {
  	    for (var j = 0; j < 9; j++) {
    	    if (this.puzzle.GetStateCR(i,j) == 0) {
        	  // No value, so print notes!
       	    var cur_x = offset + this.info.cell_width * i;
       	    var cur_y = offset + this.info.cell_width * j;

      	    ctx.font = this.mini_text_pt.toString() + 'pt Calibri';
      	    // Print the values from 1 to 9.
      	    for (var mi = 0; mi < 3; mi++) {
        	    var text_x = Math.floor(cur_x + this.info.cell_width * (0.30 * mi + 0.20));
        	    for (mj = 0; mj < 3; mj++) {
          	    var val = mi + 1 + (mj*3);
          	    if (this.puzzle.GetNoteCR(i,j,val) == false) continue;
          	  
           	    if (this.interface.auto_notes) ctx.fillStyle = "#00AA00";
          	    else ctx.fillStyle = "#006600";
          	    var text_y = cur_y + this.mini_char_width * (mj+1);
          	    ctx.fillText(val.toString(), text_x, text_y);
              }
            }	  
    	    }
    	    else {
      	    ctx.font = this.text_pt.toString() + 'pt Calibri';
      	    if (this.puzzle.GetLockCR(i,j) != 0) ctx.fillStyle = "black"; // This number is locked!
      	    else {                                                      // Changeable cell.
              var cur_id = this.puzzle.layout.ToID(i, j);
              if (this.puzzle.cell_array[cur_id].warn == true) {        // Are warnings on for this cell?
                if (bm_level[cur_id] > 0) ctx.fillStyle = "#880000";    // ...Has cell changed on the current bookmark level?
                else ctx.fillStyle = "#AA6666";                         // ...From before current bookmark
              }
              else {                                                    // No warnings
                if (bm_level[cur_id] > 0) ctx.fillStyle = "#0000FF";    // ...Has cell changed on the current bookmark level?
                else ctx.fillStyle = "#6666AA";                         // ...From before current bookmark
              }
      	    }
      	    ctx.fillText(this.puzzle.GetStateCR(i,j).toString(),
      	                 offset + this.info.cell_width * i + this.info.cell_width/2,
      	                 offset + this.info.cell_width * (j+1) - this.info.cell_padding);
          }
        }
      }

      // Give options for target cell.
      if (this.cell_id != -1 && this.puzzle.GetState(this.cell_id) == 0) {
        ctx.font = this.mini_text_pt.toString() + 'pt Calibri';
        // Print the values from 1 to 9.
        for (var i = 0; i < 3; i++) {
          var text_x = Math.floor(cell_x + this.info.cell_width * (0.30 * i + 0.20));
          for (j = 0; j < 3; j++) {
            var val = i + 1 + (j*3);
            if (this.puzzle.GetNote(this.cell_id, val) == false) {
              if (val == this.hover_val) ctx.fillStyle = "#FF8800";
              else ctx.fillStyle = "#888888";
            } else {
              if (val == this.hover_val) ctx.fillStyle = "#884400";
              else ctx.fillStyle = "#008800";              
            }
            var text_y = cell_y + this.mini_char_width * (j+1);
            ctx.fillText(val.toString(), text_x, text_y);
          }
        }	  
      }

      // Mark this region as being click-on-able.
      ctx.beginPath();
      ctx.moveTo(offset, offset);
      ctx.lineTo(offset + this.info.board_width, offset);
      ctx.lineTo(offset + this.info.board_width, offset + this.info.board_width);
      ctx.lineTo(offset, offset + this.info.board_width);
      ctx.closePath();
      canvas.fillStroke(this);
    }
  });
  
  this.puzzle_board.info = this.info;

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
        this.ctx = canvas.getContext();
 
        this.x = this.info.buttons_x[this.name];
        this.y = this.info.buttons_y;
        this.x2 = this.x + this.info.buttons_w;
        this.y2 = this.y + this.info.buttons_h;
        this.x_in = this.x + this.info.buttons_r;
        this.y_in = this.y + this.info.buttons_r;
        this.x2_in = this.x2 - this.info.buttons_r;
        this.y2_in = this.y2 - this.info.buttons_r;
  
        // Set the button color
        if (this.mouse_down == true) this.ctx.fillStyle = 'blue';
  	    else if (this.mouse_over == true) {
  	      if (this.toggle_on == true) this.ctx.fillStyle = 'rgb(250,250,200)';
  	      else this.ctx.fillStyle = 'rgb(240,240,255)';
  	    }
  	    else {
  	      if (this.toggle_on == true) this.ctx.fillStyle = 'rgb(255,255,100)';
  	      else this.ctx.fillStyle = 'rgb(255,250,245)';
  	    }
  
  	    this.ctx.beginPath();
  	    if (this.toggle_on == true) this.ctx.lineWidth = 4;
  	    else this.ctx.lineWidth = 2;
  	    if (this.round_corners.ul == true) {
    	    this.ctx.moveTo(this.x, this.y_in);
    	    this.ctx.arc(this.x_in, this.y_in, this.info.buttons_r, Math.PI, 3*Math.PI/2, false);
    	  } else this.ctx.moveTo(this.x, this.y);
    	  if (this.round_corners.ur == true) {
    	    this.ctx.arc(this.x2_in, this.y_in, this.info.buttons_r, 3*Math.PI/2, 0, false);      	  
    	  } else this.ctx.lineTo(this.x2, this.y);
    	  if (this.round_corners.lr == true) {
    	    this.ctx.arc(this.x2_in, this.y2_in, this.info.buttons_r, 0, Math.PI/2, false);      	  
  	    } else this.ctx.lineTo(this.x2, this.y2);
    	  if (this.round_corners.ll == true) {
    	    this.ctx.arc(this.x_in, this.y2_in, this.info.buttons_r, Math.PI/2, Math.PI, false);
  	    } else this.ctx.lineTo(this.x, this.y2);
  	    this.ctx.closePath();
  	    this.ctx.fill();
  	    this.ctx.stroke();
  
      	// Draw the appropriate icon.
      	// First, shift the icon to be on a 100x100 grid, and shift back afterward.
      	this.ctx.save();
        this.ctx.translate(this.x+5, this.y+5);
        this.ctx.scale((this.info.buttons_w - 10)/100, (this.info.buttons_h - 10)/100);    
  	    this.DrawIcon();
  	    //this.ctx.setTransform(1, 0, 0, 1, 0, 0);
  	    this.ctx.restore();
  
  	    // Make the button clickable (or grayed out!)
  	    this.ctx.beginPath();
  	    this.ctx.lineWidth = 2;
  	    if (this.round_corners.ul == true) {
    	    this.ctx.moveTo(this.x, this.y_in);
    	    this.ctx.arc(this.x_in, this.y_in, this.info.buttons_r, Math.PI, 3*Math.PI/2, false);
    	  } else this.ctx.moveTo(this.x, this.y);
    	  if (this.round_corners.ur == true) {
    	    this.ctx.arc(this.x2_in, this.y_in, this.info.buttons_r, 3*Math.PI/2, 0, false);      	  
    	  } else this.ctx.lineTo(this.x2, this.y);
    	  if (this.round_corners.lr == true) {
    	    this.ctx.arc(this.x2_in, this.y2_in, this.info.buttons_r, 0, Math.PI/2, false);      	  
  	    } else this.ctx.lineTo(this.x2, this.y2);
    	  if (this.round_corners.ll == true) {
    	    this.ctx.arc(this.x_in, this.y2_in, this.info.buttons_r, Math.PI/2, Math.PI, false);
  	    } else this.ctx.lineTo(this.x, this.y2);
  	    this.ctx.closePath();
  	    if (this.inactive == true) {
    	    this.ctx.fillStyle = 'rgba(200,200,200,0.5)';
    	    this.ctx.fill();
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
    this.ctx.beginPath();
    this.ctx.lineWidth = 3;
    this.ctx.fillStyle = '#F5DEB3';
    this.ctx.moveTo(5, 100);
    this.ctx.arc(15, 10, 10, Math.PI, Math.PI*1.5);
    this.ctx.arc(35, 10, 10, Math.PI*1.5, 0);
    this.ctx.lineTo(45, 100);
    this.ctx.lineTo(25, 90);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 3;
    this.ctx.shadowOffsetY = 3;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();

    // Draw Arrow 1
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(45, 30);
    this.ctx.lineTo(65, 10);
    this.ctx.lineTo(65, 20);
    this.ctx.lineTo(95, 20);
    this.ctx.lineTo(95, 40);
    this.ctx.lineTo(65, 40);
    this.ctx.lineTo(65, 50);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 3;
    this.ctx.shadowOffsetY = 3;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
    
    // Draw Arrow 2  
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(45, 75);
    this.ctx.lineTo(65, 55);
    this.ctx.lineTo(65, 65);
    this.ctx.lineTo(95, 65);
    this.ctx.lineTo(95, 85);
    this.ctx.lineTo(65, 85);
    this.ctx.lineTo(65, 95);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 3;
    this.ctx.shadowOffsetY = 3;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
  }

  this.button_bar.undo.DrawIcon = function() {
    this.ctx.beginPath();
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(30,95);              // Arrow point
    this.ctx.lineTo(50,75);               // Angle up on right side of arrow
    this.ctx.lineTo(40,75);               // Back in to base of arrow head
    this.ctx.lineTo(40,45);               // Up to bend in arrow
    this.ctx.arc(55, 45, 15, Math.PI, 0); // Lower bend
    this.ctx.lineTo(70,95);
    this.ctx.lineTo(90,95);
    this.ctx.lineTo(90,45);
    this.ctx.arc(55, 45, 35, 0, Math.PI, true);
    this.ctx.lineTo(20,75);
    this.ctx.lineTo(10,75);

    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();    
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
    
    this.ctx.beginPath();
    this.ctx.lineWidth = 3;
    this.ctx.fillStyle = 'white';
    this.ctx.rect(gridx, gridy, gridw, gridh);
    this.ctx.fill();
    for (var i = 0; i < grid_lines; i++) {
	    this.ctx.moveTo(gridx,             gridy+grid_step*i);
	    this.ctx.lineTo(gridx2,            gridy+grid_step*i);
	    this.ctx.moveTo(gridx+grid_step*i, gridy);
	    this.ctx.lineTo(gridx+grid_step*i, gridy2);
    }
    this.ctx.stroke();    

    this.ctx.beginPath();
    this.ctx.lineWidth = 3;
    this.ctx.fillStyle = '#F5DEB3';
    this.ctx.moveTo(15, 100);
    this.ctx.arc(25, 10, 10, Math.PI, Math.PI*1.5);
    this.ctx.arc(45, 10, 10, Math.PI*1.5, 0);
    this.ctx.lineTo(55, 100);
    this.ctx.lineTo(35, 90);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 3;
    this.ctx.shadowOffsetY = 3;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
  }

  this.button_bar.redo.DrawIcon = function() {
    this.ctx.beginPath();
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(70,95);              // Arrow point
    this.ctx.lineTo(50,75);               // Angle up on right side of arrow
    this.ctx.lineTo(60,75);               // Back in to base of arrow head
    this.ctx.lineTo(60,45);               // Up to bend in arrow
    this.ctx.arc(45, 45, 15, 0, Math.PI, true); // Lower bend
    this.ctx.lineTo(30,95);
    this.ctx.lineTo(10,95);
    this.ctx.lineTo(10,45);
    this.ctx.arc(45, 45, 35, Math.PI, 0, false);
    this.ctx.lineTo(80,75);
    this.ctx.lineTo(90,75);

    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();    
  }
  
  this.button_bar.redoall.DrawIcon = function() {
    // Arrow 1
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(70, 30);
    this.ctx.lineTo(50, 10);
    this.ctx.lineTo(50, 20);
    this.ctx.lineTo(5, 20);
    this.ctx.lineTo(5, 40);
    this.ctx.lineTo(50, 40);
    this.ctx.lineTo(50, 50);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();

    // Arrow 2
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.fillStyle = 'white';
    this.ctx.moveTo(70, 75);
    this.ctx.lineTo(50, 55);
    this.ctx.lineTo(50, 65);
    this.ctx.lineTo(5, 65);
    this.ctx.lineTo(5, 85);
    this.ctx.lineTo(50, 85);
    this.ctx.lineTo(50, 95);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();

    // Wall
    this.ctx.beginPath();
    this.ctx.rect(75, 0, 10, 100);
    this.ctx.fillStyle = '#999';
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
  }
  
  this.button_bar.hint.DrawIcon = function() {
    this.ctx.font = '95pt Helvetica';
    this.ctx.fillStyle = 'green';
    this.ctx.textAlign = 'center';
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fillText('?', 50, 95);
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
  }
  
  this.button_bar.warnings.DrawIcon = function() {
    this.ctx.font = '95pt Helvetica';
    this.ctx.fillStyle = '#800';
    this.ctx.textAlign = 'center';
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fillText('!', 45, 95);
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();
  }
  
  this.button_bar.autonotes.DrawIcon = function() {
    // Board
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.moveTo(10, 0);
    this.ctx.lineTo(10, 100);
    this.ctx.moveTo(90, 0);
    this.ctx.lineTo(90, 100);
    this.ctx.moveTo(0, 10);
    this.ctx.lineTo(100, 10);
    this.ctx.moveTo(0, 90);
    this.ctx.lineTo(100, 90);
    this.ctx.stroke();

    // Pencil marks    
    this.ctx.font = '20pt Helvetica';
    this.ctx.fillStyle = '#008800';
    this.ctx.textAlign = 'center';
    this.ctx.fillText('?', 25, 35);
    this.ctx.fillText('?', 50, 35);
    this.ctx.fillText('?', 75, 35);
    this.ctx.fillText('?', 25, 60);
    this.ctx.fillText('?', 50, 60);
    this.ctx.fillText('?', 75, 60);
    this.ctx.fillText('?', 25, 85);
    this.ctx.fillText('?', 50, 85);
    this.ctx.fillText('?', 75, 85);
    this.ctx.stroke();
  }
  
  this.button_bar.toggleclick.DrawIcon = function() {
    // Board
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.moveTo(10, 0);
    this.ctx.lineTo(10, 100);
    this.ctx.moveTo(90, 0);
    this.ctx.lineTo(90, 100);
    this.ctx.moveTo(0, 10);
    this.ctx.lineTo(100, 10);
    this.ctx.moveTo(0, 90);
    this.ctx.lineTo(100, 90);
    this.ctx.stroke();

    // Pencil marks    
    this.ctx.font = '25pt Helvetica';
    this.ctx.fillStyle = 'blue';
    this.ctx.textAlign = 'center';
    this.ctx.fillText('1', 25, 40);
    this.ctx.fillText('4', 75, 62);
    this.ctx.fillText('7', 25, 85);
    this.ctx.stroke();

    // Pencil
    this.ctx.beginPath();
    this.ctx.lineWidth = 2;
    this.ctx.fillStyle = '#F5DEB3';
    this.ctx.moveTo(30, 85);
    this.ctx.lineTo(30, 70);
    this.ctx.lineTo(60, 0);
    this.ctx.lineTo(70, 5);
    this.ctx.lineTo(40, 75);
    this.ctx.closePath();
    this.ctx.shadowColor = "#999";
    this.ctx.shadowBlur = 4;
    this.ctx.shadowOffsetX = 2;
    this.ctx.shadowOffsetY = 2;
    this.ctx.fill();
    this.ctx.shadowColor = "transparent";
    this.ctx.stroke();    
    this.ctx.beginPath();
    this.ctx.lineWidth = 1;
    this.ctx.moveTo(30,70);
    this.ctx.lineTo(40,75);
    this.ctx.stroke();
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

  this.Resize = function() {
    var info = this.info;
    var pb = this.puzzle_board;  // @CAO Move these to info??
  
    var win_size = Math.min(window.innerWidth * 1.25, window.innerHeight)
    win_size = Math.max(win_size, info.min_size);
    info.puzzle_size = win_size * 0.8;

    // These are dynamically sized numbers.
    info.cell_width = Math.floor( (info.puzzle_size - info.border*2)/9 );  
    info.region_width = info.cell_width * 3;                                
    info.board_width = info.cell_width * 9;                                 
    pb.text_pt = info.cell_width - 2 * info.cell_padding;                 // Font size for text filling a cell.
    pb.mini_char_width = Math.floor(info.cell_width / 3) - 1;             // Space for smaller characters in each cell
    pb.mini_text_pt = pb.mini_char_width - 1;                           // Font size for smaller characters
    info.full_width = info.board_width + info.border*2;

    info.buttons_w = info.full_width/10;     // Width of each button
    info.buttons_h = info.buttons_w;          // Height of each button (for now, keep them square)
    var offset = 3;
    info.buttons_x['rewind']      = offset;
    info.buttons_x['undo']        = offset + info.buttons_w;
    info.buttons_x['bookmark']    = offset + info.buttons_w * 2;
    info.buttons_x['redo']        = offset + info.buttons_w * 3;
    info.buttons_x['redoall']     = offset + info.buttons_w * 4;
    info.buttons_x['hint']        = info.buttons_w * 5.5;
    info.buttons_x['warnings']    = info.buttons_w * 7 - offset;
    info.buttons_x['autonotes']   = info.buttons_w * 8 - offset;
    info.buttons_x['toggleclick'] = info.buttons_w * 9 - offset;
    info.buttons_y = info.puzzle_size + 2;    // Y-value of main button bar
  }

  this.Resize();


//////////////////////////
//  Construct the Layers
//////////////////////////

  // Setup the puzzle layer.
//!
  this.layer_main.add(this.puzzle_board);
  this.stage.add(this.layer_main);

  // Setup the button layer
//!  this.layer_buttons = new Kinetic.Layer();
  for (cur_but in this.button_bar) {
    this.layer_buttons.add(this.button_bar[cur_but]);
  }
  this.stage.add(this.layer_buttons);

  // Setup the tooltip layer
  this.layer_tooltips = new Kinetic.Layer();
  this.layer_tooltips.add(this.tooltip_box);
  this.layer_tooltips.add(this.tooltip_text);
  this.stage.add(this.layer_tooltips);
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
