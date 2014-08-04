#ifndef EMK_CANVAS_TOOLS_H
#define EMK_CANVAS_TOOLS_H

#include "../libs/Kinetic.h"

namespace emk {

  static void DrawRoundedRect(Canvas & canvas, int x, int y, int width, int height, int corner_r, bool fill=false, bool stroke=true,
                              bool ul_round=true, bool ur_round=true, bool lr_round=true, bool ll_round=true)
  {
    const int x2 = x + width;          // Far X coordinate
    const int y2 = y + height;         // Far Y coordinate
    const int x_in = x + corner_r;     // Inner x for corner arcs
    const int y_in = y + corner_r;     // Inner y for corner arcs
    const int x2_in = x2 - corner_r;   // Inner x2 for corner arcs
    const int y2_in = y2 - corner_r;   // Inner y2 for corner arcs
    
    // Draw the button outline
    canvas.BeginPath();

    if (ul_round) {
      canvas.MoveTo(x, y_in);
      canvas.Arc(x_in, y_in, corner_r, emk::PI, 3*emk::PI/2);
    } else canvas.MoveTo(x, y);

    if (ur_round) canvas.Arc(x2_in, y_in, corner_r, 3*emk::PI/2, 0);      	  
    else canvas.LineTo(x2, y);

    if (lr_round) canvas.Arc(x2_in, y2_in, corner_r, 0, emk::PI/2);      	  
    else canvas.LineTo(x2, y2);

    if (ll_round) canvas.Arc(x_in, y2_in, corner_r, emk::PI/2, emk::PI);
    else canvas.LineTo(x, y2);

    canvas.ClosePath();
    if (fill) canvas.Fill();
    if (stroke) canvas.Stroke();
  }

};

#endif
