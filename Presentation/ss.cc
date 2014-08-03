#include <emscripten.h>
#include <iostream>
#include <cmath>

#include <html5.h>

#include "../libs/Kinetic.h"
#include "../tools/assert.h"
#include "../tools/Random.h"

#include "../cogs/Control.h"
#include "../cogs/SlideShow.h"

emk::SlideShow show;

extern "C" int emkMain()
{
  // @CAO Setup background slides...
  int stage_w = show.Stage().GetWidth() - 12;
  int stage_h = show.Stage().GetHeight() - 12;  
  show.BuildRect("white_bg", 5, 5, stage_w, stage_h, "white", "black", 3);
  show.Appear("white_bg");

  // TITLE
  show << show(0.1, 0.08) << emk::Font(stage_w/15) << "["
       << emk::Color("blue") << "Digital"
       << emk::Color("black") << "] Evolution in Action";

  // Personal Info
  show << show(0.45, 0.35) << emk::Font(stage_w/25) << "Charles Ofria\nMichigan State University";
  show << show(0.45, 0.50) << emk::Font(stage_w/35) << "Professor of Computer Science\nDeputy Director, BEACON Center";

  show.Pause();

  const int logo_scale = show.ScaleY(0.6);
  show << show.BuildImage("image_beacon", "../icons/BeaconLogo.png", show.ScaleX(0.03), show.ScaleY(0.35), logo_scale, logo_scale);
  //  emk::Image & image_beacon = show.BuildImage("image_beacon", "../icons/BeaconLogo.png", show.ScaleX(0.03), show.ScaleY(0.35), logo_scale, logo_scale);
  //  show.Appear(image_beacon);


  show.NewSlide();

  emk::Rect & rect = show.BuildRect("test", 50, 50, 100, 100);
  show.Appear(rect);

  show.Pause();

  emk::Rect & rect2 = show.BuildRect("test2", 250, 250, 100, 100, "red", "black", 5, true);
  show.Appear(rect2);

  show.NewSlide();

  emk::Rect & rect3 = show.BuildRect("test3", 200, 200, 100, 100, "blue", "black", 5, true);
  show.Appear(rect3);

  show.Pause();
  show.Appear(rect);

  show.Start();

  return 0;
}
