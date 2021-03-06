#include <emscripten.h>
#include <iostream>
#include <cmath>

#include <html5.h>

#include "../../libs/Kinetic.h"
#include "../../tools/assert.h"
#include "../../tools/Random.h"

#include "../../cogs/Control.h"
#include "../../cogs/SlideShow.h"

emk::SlideShow show;

extern "C" int emkMain()
{
  // @CAO Setup background slides...
  int stage_w = show.Stage().GetWidth() - 12;
  int stage_h = show.Stage().GetHeight() - 12;  
  show.BuildRect("white_bg", emk::Point(5, 5), stage_w, stage_h, "white", "black", 3);
  show.AddBackground(show.Rect());

  // ---=== TITLE Slide ===---
  show.NewSlide(); // Create the title slide.
  show << show(0.1, 0.08) << emk::Font(stage_w/15) << "Evolution in Action Software";
  show.Text().SetCenter(show(0.5, 0.12));
  show << "and the Web";
  show.Text().SetCenter(show(0.5, 0.24));

  show << show(0.45, 0.45) << emk::Font(stage_w/25) << "Charles Ofria\nMichigan State University";
  show << show(0.45, 0.60) << emk::Font(stage_w/35) << "Professor of Computer Science\nDeputy Director, BEACON Center";

  const int logo_size = show.ScaleY(0.6);
  show << emk::Image("images/BeaconLogo.png", show(0.03, 0.35), logo_size, logo_size);

  // ---=== My Thought Process ===---
  show.NewSlide("What's been done?");

  show.Pause();

  // show << emk::Image("images/boxcar.jpg", show(0.1,0.3)).SetScale(1.25);  // @CAO Why does this not work??
  show << show.BuildImage("boxcar", "images/boxcar.jpg", show(0.1,0.3)).SetScale(1.25);

  show.Pause();
  show << emk::Tween(show.Image(), 3).SetXY( show(0.7, 0.7) ).SetScale(0.25).SetEasing(emk::Tween::StrongEaseOut);



  // ---=== Evolution is Hard to Study! ===---


  show.NewSlide("Evolution is hard to study!");


  emk::Rect & rect = show.BuildRect("test", emk::Point(50, 50), 100, 100);
  show.Appear(rect);

  show.Pause();

  emk::Rect & rect2 = show.BuildRect("test2", emk::Point(250, 250), 100, 100, "red", "black", 5, true);
  show.Appear(rect2);

  show.NewSlide();

  emk::Rect & rect3 = show.BuildRect("test3", emk::Point(200, 200), 100, 100, "blue", "black", 5, true);
  show.Appear(rect3);

  show.Pause();
  show.Appear(rect);

  show.Start();

  return 0;
}
