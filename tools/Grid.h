#ifndef EMK_GRID_H
#define EMK_GRID_H

#include "../Kinetic.h"

#include <algorithm>

namespace emk {
  class Grid : public emkCustomShape {
  private:
    int width;
    int height;
    const int num_cols;
    const int num_rows;
    int border_width;

  public:
    Grid(int _x, int _y, int _width, int _height, int _cols, int _rows, int _border_width=1)
      : emkCustomShape(_x, _y, this, &Grid::Draw)
      , width(_width), height(_height), num_cols(_cols), num_rows(_rows), border_width(_border_width)
    { ; }
    ~Grid() { ; }

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

          canvas.SetFillStyle("black");
          canvas.Rect(x_pos, y_pos, cell_width, cell_height, true);
        }
      }

      canvas.Draw();
    }
  };
}

#endif
