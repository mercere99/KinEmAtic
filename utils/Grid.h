#ifndef EMK_GRID_H
#define EMK_GRID_H

#include "../tools/Colors.h"
#include "../tools/ProbSchedule.h"
#include "../Kinetic.h"

#include <vector>

namespace emk {
  class Grid;

  class GridPointer : public emkCustomShape {
  private:
    const Grid & main_grid;
    emk::Color outer_color;
    emk::Color inner_color;
  public:
    GridPointer(const Grid & _main, const emk::Color & _outer, const emk::Color & _inner)
      : emkCustomShape(0, 0, 0, 0, this, &GridPointer::Draw)
      , main_grid(_main)
      , outer_color(_outer), inner_color(_inner)
    { ; }
    ~GridPointer() { ; }

    void SetColors(const emk::Color & _outer, const emk::Color & _inner) {
      outer_color = _outer;
      inner_color = _inner;
    }

    void Draw(emkCanvas & canvas) {
      const int width = GetWidth();
      const int height = GetHeight();
      canvas.SetStroke(outer_color);
      canvas.Rect(0, 0, width, height, false);
      canvas.SetStroke(inner_color);
      canvas.Rect(1, 1, width-2, height-2, false);
      canvas.Stroke();
    }    
  };

  class Grid : public emkCustomShape {
  private:
    const int num_cols;
    const int num_rows;
    const int num_cells;
    int num_colors;
    int border_width;
    
    std::vector<int> grid_colors;
    emk::ColorMap color_map;

    // Calculated values
    int cell_x_space;
    int cell_y_space;
    int cell_width;
    int cell_height;
    int grid_width;
    int grid_height;
 
    // UI Values
    int mouse_col;
    int mouse_row;

    GridPointer mouse_pointer;

    inline int CellID(int row, int col) const { return row * num_cols + col; }
  public:
    Grid(int _x, int _y, int _width, int _height, int _cols, int _rows, int _num_colors=12, int _border_width=1)
      : emkCustomShape(_x, _y, _width, _height, this, &Grid::Draw)
      , num_cols(_cols), num_rows(_rows), num_cells(num_rows*num_cols)
      , num_colors(_num_colors), border_width(_border_width), grid_colors(num_cells), color_map(num_colors, true)
      , mouse_col(-1), mouse_row(-1), mouse_pointer(*this, "yellow", "black")
    {
      SetupSize();
      for (int i = 0; i < num_cells; i++) grid_colors[i] = 0;
      On("click", this, &Grid::OnClick);
      On("mousemove", this, &Grid::OnMousemove);
      On("mouseout", this, &Grid::OnMouseout);
      On("mouseover", this, &Grid::OnMouseover);
    }
    ~Grid() { ; }

    void SetupSize() {
      // For the moment, grid cells can only be whole pixels on a side.
      cell_x_space = (GetWidth() - border_width) / num_cols;
      cell_y_space = (GetHeight() - border_width) / num_rows;
      grid_width = cell_x_space * num_cols + border_width;
      grid_height = cell_y_space * num_rows + border_width;
      cell_width = cell_x_space - border_width;
      cell_height = cell_y_space - border_width;

      mouse_pointer.SetSize(cell_width, cell_height);
    }

    inline int GetColor(int id) const {
      return grid_colors[id];
    }
    int GetColor(int row, int col) const {
      return GetColor(CellID(row, col));
    }
    int GetNumCols() const { return num_cols; }
    int GetNumRows() const { return num_rows; }
    int GetNumCells() const { return num_cells; }
    
    int GetMouseCol() const { return mouse_col; }
    int GetMouseRow() const { return mouse_row; }
    int GetMouseCellID() const { return (mouse_row == -1) ? -1 : mouse_row * num_cols + mouse_col; }
    GridPointer & GetMousePointer() { return mouse_pointer; }

    inline void SetColor(int id, int color) {
      grid_colors[id] = color;
    }
    void SetColor(int row, int col, int color) { SetColor(CellID(row, col), color); }

    void Draw(emkCanvas & canvas) {
      // Setup the background black...
      canvas.SetFillStyle("black");
      canvas.Rect(0, 0, grid_width, grid_height, true);

      // Draw each individual cell.
      for (int col = 0; col < num_cols; col++) {
        const int x_pos = border_width + cell_x_space * col;
        for (int row = 0; row < num_rows; row++) {
          const int y_pos = border_width + cell_y_space * row;
          const int id = col + row * num_cols;
          
          canvas.SetFillStyle(color_map[grid_colors[id]].AsString());
          canvas.Rect(x_pos, y_pos, cell_width, cell_height, true);
        }
      }

      // Highlight the cursor under the mouse.
      if (mouse_col >= 0 && false) {
        const int x_pos = border_width + cell_x_space * mouse_col;
        const int y_pos = border_width + cell_y_space * mouse_row;
        canvas.SetStroke("yellow");
        canvas.Rect(x_pos, y_pos, cell_width, cell_height, false);
        canvas.SetStroke("black");
        canvas.Rect(x_pos+1, y_pos+1, cell_width-2, cell_height-2, false);
      }
      

      canvas.Stroke();

      // Make the canvas respond to the mouse.
      canvas.BeginPath();
      canvas.SetLineWidth(1);
      canvas.MoveTo(0, 0);
      canvas.LineTo(grid_width, 0);
      canvas.LineTo(grid_width, grid_height);
      canvas.LineTo(0, grid_height);
      canvas.ClosePath();
      canvas.SetFillStyle(emk::Color(255, 0, 0, 0.5));
      canvas.SetupTarget(*this);
    }

    void OnClick(const emkEventInfo & evt) {
      emkAlert("Click!!!");
    }
                                               
    void OnMousemove(const emkEventInfo & evt) {
      const int mouse_x = evt.layer_x - GetX();
      const int mouse_y = evt.layer_y - GetY();
      mouse_col = num_cols * mouse_x / (grid_width - border_width);
      if (mouse_col >= num_cols) mouse_col = num_cols-1;
      mouse_row = num_rows * mouse_y / (grid_height - border_width);
      if (mouse_row >= num_rows) mouse_row = num_rows-1;
      const int x_pos = border_width + cell_x_space * mouse_col;
      const int y_pos = border_width + cell_y_space * mouse_row;
      mouse_pointer.SetXY(x_pos + GetX(), y_pos + GetY());
      mouse_pointer.DrawLayer();
    }
                                               
    void OnMouseout() {  // (const emkEventInfo & evt) {
      mouse_col = mouse_row = -1;  // Don't highlight any cells if the mouse isn't on the grid.
    }
                                               
    void OnMouseover() { // (const emkEventInfo & evt) {
    }
                                               
  };
}

#endif
