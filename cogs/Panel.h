#ifndef EMK_PANEL_H
#define EMK_PANEL_H

#include "../libs/Kinetic.h"

namespace emk {

  class Panel : public CustomShape {
  private:
  public:
    Panel(int _x, int _y, int _width, int _height)
      : CustomShape(_x, _y, _width, _height, this, &Panel::Draw)
    { ; }

    void Draw(Canvas & canvas) {
      // Setup the background black...
      canvas.SetFillStyle("#AAAAAA");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), true);
      canvas.SetStroke("#000000");
      canvas.Rect(0, 0, GetWidth(), GetHeight(), false);
    }

  };

};

#endif
