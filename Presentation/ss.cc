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
  show.AddRect("white_bg", 5, 5, stage_w, stage_h, "white", "black", 3);
  show.Appear("white_bg");

  show << "This is a test!!!" << emk::Font("Helvetica", 80, "red") << "  ... and so it this!";

  show.AddFont("title", emk::Font()).SetSize(stage_w/15);
  emk::Text & title = show.AddText("title", 0, stage_w/12, "[          ] Evolution in Action");
  const int title_x = (stage_w - title.GetWidth())/2;
  title.SetX(title_x);
  emk::Text bracket(0, 0, "[");
  emk::Text & title_blue = show.AddText("title_blue", title_x + bracket.GetWidth(), stage_w/12, "Digital");
  title_blue.SetFill("blue");
  show.Appear("title");
  show.Appear("title_blue");

  show.AddFont("my_info", emk::Font()).SetSize(stage_w/25);
  emk::Text & my_info = show.AddText("my_info", show.ScaleX(0.45), show.ScaleY(0.35), "Charles Ofria\nMichigan State University");
  show.Appear(my_info);

  show.AddFont("my_info2", emk::Font()).SetSize(stage_w/35).SetColor(emk::Color(0,0,64));
  emk::Text & my_info2 = show.AddText("my_info2", show.ScaleX(0.45), show.ScaleY(0.50), "Professor of Computer Science\nDeputy Director, BEACON Center");
  show.Appear(my_info2);

  show.Pause();

  const int logo_scale = show.ScaleY(0.6);
  emk::Image & image_beacon = show.AddImage("image_beacon", "../icons/BeaconLogo.png", show.ScaleX(0.03), show.ScaleY(0.35), logo_scale, logo_scale);
  show.Appear(image_beacon);


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
