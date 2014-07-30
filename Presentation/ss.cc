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
  // Load all images upfront and then pause before starting show... ?
  // emk::Image image_beacon("BeaconLogo.jpg");
  // emk::Image image_beacon("../icons/BeaconLogo.png");
  int stage_w = show.Stage().GetWidth() - 12;
  int stage_h = show.Stage().GetHeight() - 12;
  
  show.AddRect("white_bg", 5, 5, stage_w, stage_h, "white", "black", 3);
  show.Appear("white_bg");

  const int title_size = stage_w/15;
  emk::Text & title = show.AddText("title", 0, stage_w/12, "[          ] Evolution in Action", title_size, "Helvetica");
  const int title_x = (stage_w - title.GetWidth())/2;
  title.SetX(title_x);
  emk::Text bracket(0, 0, "[", title_size, "Helvetica");
  emk::Text & title_blue = show.AddText("title_blue", title_x + bracket.GetWidth(), stage_w/12, "Digital", title_size, "Helvetica");
  title_blue.SetFill("blue");
  show.Appear("title");
  show.Appear("title_blue");

  show.Pause();

  emk::Rect & logo = show.AddRect("beacon_logo", 10, 10, 100, 100, "white", "black", 4);
  // show.Appear(logo, image_beacon);


  show.NewSlide();

  emk::Rect & rect = show.AddRect("test", 50, 50, 100, 100);
  show.Appear(rect);

  show.Pause();

  emk::Rect & rect2 = show.AddRect("test2", 250, 250, 100, 100, "red", "black", 5, true);
  show.Appear(rect2);

  show.NewSlide();

  emk::Rect & rect3 = show.AddRect("test3", 200, 200, 100, 100, "blue", "black", 5, true);
  show.Appear(rect3);

  show.Pause();
  show.Appear(rect);

  show.Start();

  return 0;
}
