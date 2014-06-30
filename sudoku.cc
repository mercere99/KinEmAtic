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



class KineticExample {
private:
  emkStage stage;
  emkLayer layer_anim;
  emkLayer layer_static;

  emkRegularPolygon blue_hex;
  emkRegularPolygon yellow_hex;
  emkRegularPolygon red_hex;
  emkText text;

  emkAnimation<KineticExample> anim;


public:
  KineticExample()
    : stage(578, 200, "container")
    , blue_hex(100, stage.GetHeight()/2, 7, 70, "#00D2FF", "black", 10, true)
    , yellow_hex(stage.GetWidth()/2, stage.GetHeight()/2, 6, 70, "yellow", "red", 4, true)
    , red_hex(470, stage.GetHeight()/2, 6, 70, "red", "black", 4, true)
    , text(10, 10, "Static Layer C++", "30", "Calibri", "black")
  {
    layer_anim.Add(blue_hex);
    layer_anim.Add(yellow_hex);
    layer_anim.Add(red_hex);
    layer_static.Add(text);
    red_hex.SetOffset(70, 0);
    stage.Add(layer_anim).Add(layer_static);

    anim.Setup(this, &KineticExample::Animate, layer_anim);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    const double PI = 3.141592653589793238463;
    const double period = 2000.0;
    const double scale = std::sin(frame.time * 2 * PI / period) + 0.001;

    blue_hex.SetScale(scale, scale);    // scale x and y
    yellow_hex.SetScale(1.0, scale);    // scale only x
    red_hex.SetScale(scale, 1.0);       // scale only y
  }
};



KineticExample * example;

extern "C" int emkMain()
{
  example = new KineticExample();

  return 0;
}
