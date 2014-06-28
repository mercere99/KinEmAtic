#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"

using namespace std;

class KineticExample {
private:
  emkStage stage;
  emkLayer layer_anim;
  emkLayer layer_static;

  emkRegularPolygon blue_hex;
  emkRegularPolygon yellow_hex;
  emkRegularPolygon red_hex;
  emkText text;

  emkAnimation<KineticExample> anim;


public:
  KineticExample()
    : stage(578, 200, "container")
    , blue_hex(100, stage.GetHeight()/2, 7, 70, "#00D2FF", "black", 10, true)
    , yellow_hex(stage.GetWidth()/2, stage.GetHeight()/2, 6, 70, "yellow", "red", 4, true)
    , red_hex(470, stage.GetHeight()/2, 6, 70, "red", "black", 4, true)
    , text(10, 10, "Static Layer C++", "30", "Calibri", "black")
  {
    layer_anim.Add(blue_hex);
    layer_anim.Add(yellow_hex);
    layer_anim.Add(red_hex);
    layer_static.Add(text);
    red_hex.SetOffset(70, 0);
    stage.Add(layer_anim).Add(layer_static);

    anim.Setup(this, &KineticExample::Animate, layer_anim);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    const double PI = 3.141592653589793238463;
    const double period = 2000.0;
    const double scale = std::sin(frame.time * 2 * PI / period) + 0.001;

    blue_hex.SetScale(scale, scale);    // scale x and y
    yellow_hex.SetScale(1.0, scale);    // scale only x
    red_hex.SetScale(scale, 1.0);       // scale only y
  }
};



KineticExample * example;

extern "C" int emkMain()
{
  example = new KineticExample();

  return 0;
}
