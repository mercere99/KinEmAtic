#ifndef EMK_GRID_H
#define EMK_GRID_H

#include "../tools/Colors.h"
#include "../Kinetic.h"

#include <vector>

namespace emk {
  class Grid : public emkCustomShape {
  private:
    const int num_cols;
    const int num_rows;
    const int num_cells;
    int num_colors;
    int border_width;
    
    std::vector<int> grid_colors;
    emk::ColorMap color_map;

    inline int CellID(int row, int col) const { return row * num_cols + col; }
  public:
    Grid(int _x, int _y, int _width, int _height, int _cols, int _rows, int _num_colors=12, int _border_width=1)
      : emkCustomShape(_x, _y, _width, _height, this, &Grid::Draw)
      , num_cols(_cols), num_rows(_rows), num_cells(num_rows*num_cols)
      , num_colors(_num_colors), border_width(_border_width), grid_colors(num_cells), color_map(num_colors, true)
    {
      for (int i = 0; i < num_cells; i++) grid_colors[i] = 0;
    }
    ~Grid() { ; }

    inline int GetColor(int id) const {
      return grid_colors[id];
    }
    int GetColor(int row, int col) const {
      return GetColor(CellID(row, col));
    }
    int GetNumCols() const { return num_cols; }
    int GetNumRows() const { return num_rows; }
    int GetNumCells() const { return num_cells; }

    inline void SetColor(int id, int color) {
      grid_colors[id] = color;
    }
    void SetColor(int row, int col, int color) { SetColor(CellID(row, col), color); }

    void Draw(emkCanvas & canvas) {
      // For the moment, grid cells can only be whole pixels on a side.
      
      const int cell_x_space = (GetWidth() - border_width) / num_cols;
      const int cell_y_space = (GetHeight() - border_width) / num_rows;
      
      // Setup the background black...
      const int grid_width = cell_x_space * num_cols + border_width;
      const int grid_height = cell_y_space * num_rows + border_width;
      canvas.SetFillStyle("black");
      canvas.Rect(0, 0, grid_width, grid_height, true);

      // Draw each individual cell.
      const int cell_width = cell_x_space - border_width;
      const int cell_height = cell_y_space - border_width;

      for (int col = 0; col < num_cols; col++) {
        const int x_pos = border_width + cell_x_space * col;
        for (int row = 0; row < num_rows; row++) {
          const int y_pos = border_width + cell_y_space * row;
          const int id = col + row * num_cols;
          
          canvas.SetFillStyle(color_map[grid_colors[id]].AsString());
          canvas.Rect(x_pos, y_pos, cell_width, cell_height, true);
        }
      }

      canvas.Stroke();
    }
  };
}

#endif
