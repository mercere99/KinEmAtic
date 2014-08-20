#ifndef EMK_BUTTON_SET_H
#define EMK_BUTTON_SET_H

#include "Button.h"

#include <vector>

namespace emk {
  
  class ButtonGrid : public ObjectGrid<Button> {
  private:
    int button_width;
    int button_height;
    int spacing;

    int set_size;
    std::vector<Button *> button_set;

  public:
    ButtonGrid(int _cols, int _rows, int _x, int _y, int _width, int _height, int _spacing=0)
      : ObjectGrid(_cols, _rows, _x, _y)
      , button_width(_width), button_height(_height), spacing(_spacing),
        set_size(cols * rows), button_set(set_size)
    {
      const int extra_width = button_width + spacing;  // Gives room for all buttons to have spacing on right.
      const int extra_height = button_height + spacing;  // Gives room for all buttons to have a spacing below.

      for (int i = 0; i < cols; i++) {
        const int cur_x = x + extra_width * i;
        for (int j = 0; j < rows; j++) {
          const int cur_y = y + extra_height * j;
          Button * new_button = new Button();
          new_button->SetLayout(cur_x, cur_y, button_width, button_height);
          button_set[i + j * cols] = new_button;
        }
      }

    }
    ButtonGrid(int _cols, int _rows, const Point & point, int _width, int _height, int _spacing=0)
      : ButtonGrid(_cols, _rows, point.GetX(), point.GetY(), _width, _height, _spacing) { ; }

    ~ButtonGrid() {
      for (int i = 0; i < (int) button_set.size(); i++) {
        if (button_set[i]) delete button_set[i];
      }
    }

    int GetWidth() const { return cols * (button_width + spacing) - spacing; }
    int GetHeight() const { return rows * (button_height + spacing) - spacing; }

    void AddToLayer(Layer & layer) { for (int i = 0; i < set_size; i++) layer.Add(*(button_set[i]));  }

    Button & Get(int pos) { return *(button_set[pos]); }
    Button & Get(int col, int row) { return *(button_set[col + row*cols]); }
    Button & operator[](int pos) { return *(button_set[pos]); }

    ButtonGrid & SetActive(bool active=true) { for (int i = 0; i < set_size; i++) { button_set[i]->SetActive(active); } return *this; }
    ButtonGrid & SetRoundCorners(bool ul=true, bool ur=true, bool lr=true, bool ll=true) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetRoundCorners(ul, ur, lr, ll); }
      return *this;
    }
    void AutoRoundCorners() {
      for (int i = 0; i < set_size; i++) button_set[i]->SetRoundCorners(false, false, false, false);
      button_set[0]->SetRoundCornerUL();
      button_set[cols-1]->SetRoundCornerUR();
      button_set[(rows-1) * cols + 1]->SetRoundCornerLR();
      button_set[set_size-1]->SetRoundCornerLL();
    }
    ButtonGrid & SetFillPatternImage(const Image & image) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetFillPatternImage(image); }
      return *this;
    }

    ButtonGrid & SetBGColor(const Color & color) { 
      for (int i = 0; i < set_size; i++) { button_set[i]->SetBGColor(color); }
      return *this;
    }
    ButtonGrid & SetBGColorToggled(const Color & color) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetBGColorToggled(color); }
      return *this;
    }
    ButtonGrid & SetBGColorMouseover(const Color & color) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetBGColorMouseover(color); }
      return *this;
    }
    ButtonGrid & SetBGColorToggledMouseover(const Color & color) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetBGColorToggledMouseover(color); }
      return *this;
    }
    ButtonGrid & SetBGColorMousedown(const Color & color) {
      for (int i = 0; i < set_size; i++) { button_set[i]->SetBGColorMousedown(color); }
      return *this;
    }

    Point GetUL(int x_offset=0, int y_offset=0) const { return Point(x+x_offset,               y+y_offset); }
    Point GetUM(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()/2+x_offset,  y+y_offset); }
    Point GetUR(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()+x_offset,    y+y_offset); }
    Point GetML(int x_offset=0, int y_offset=0) const { return Point(x+x_offset,               y+GetHeight()/2+y_offset); }
    Point GetMM(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()/2+x_offset,  y+GetHeight()/2+y_offset); }
    Point GetMR(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()+x_offset,    y+GetHeight()/2+y_offset); }
    Point GetLL(int x_offset=0, int y_offset=0) const { return Point(x+x_offset,               y+GetHeight()+y_offset); }
    Point GetLM(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()/2+x_offset,  y+GetHeight()+y_offset); }
    Point GetLR(int x_offset=0, int y_offset=0) const { return Point(x+GetWidth()+x_offset,    y+GetHeight()+y_offset); }
    Point GetCenter(int x_offset=0, int y_offset=0) const { return GetMM(x_offset, y_offset); }
  };
  
};

#endif
