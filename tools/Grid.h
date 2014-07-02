#ifndef EMK_GRID_H
#define EMK_GRID_H

#include "../Kinetic.h"

#include <vector>

namespace emk {
  class Grid : public emkCustomShape {
  private:
    int width;
    int height;
    const int num_cols;
    const int num_rows;
    const int num_cells;
    int border_width;
    
    std::vector<int> grid_colors;
    std::vector<std::string> color_map;

    inline int CellID(int row, int col) const { return row * num_cols + col; }
  public:
    Grid(int _x, int _y, int _width, int _height, int _cols, int _rows, int _border_width=1)
      : emkCustomShape(_x, _y, this, &Grid::Draw)
      , width(_width), height(_height), num_cols(_cols), num_rows(_rows), num_cells(num_rows*num_cols)
      , border_width(_border_width), grid_colors(num_cells)
    {
      // @CAO For testing purposes, build a static color map and start everyone as black.
      color_map.push_back("black");
      color_map.push_back("gray");
      color_map.push_back("red");
      color_map.push_back("blue");
      color_map.push_back("green");

      for (int i = 0; i < num_cells; i++) grid_colors[i] = 0;
    }
    ~Grid() { ; }

    int GetColor(int id) const {
      return grid_colors[id];
    }
    int GetColor(int row, int col) const {
      return grid_colors[CellID(row, col)];
    }
    int GetNumCols() const { return num_cols; }
    int GetNumRows() const { return num_rows; }
    int GetNumCells() const { return num_cells; }

    void SetColor(int id, int color) {
      grid_colors[id] = color;
    }
    void SetColor(int row, int col, int color) {
      grid_colors[CellID(row, col)] = color;
    }

    void Draw(emkCanvas & canvas) {
      // For the moment, grid cells can only be whole pixels on a side.
      const int cell_x_space = width / num_cols;
      const int cell_y_space = height / num_rows;
      
      const int cell_width = cell_x_space - border_width;
      const int cell_height = cell_y_space - border_width;

      for (int col = 0; col < num_cols; col++) {
        const int x_pos = x + cell_x_space * col;
        for (int row = 0; row < num_rows; row++) {
          const int y_pos = y + cell_y_space * row;
          const int id = col + row * num_cols;

          canvas.SetFillStyle(color_map[grid_colors[id]]);
          canvas.Rect(x_pos, y_pos, cell_width, cell_height, true);
        }
      }

      canvas.Draw();
    }
  };
}

#endif
