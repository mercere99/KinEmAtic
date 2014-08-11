#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "../libs/Kinetic.h"

using namespace std;

class KineticExample {
private:
  emk::Stage stage;
  emk::Layer layer;

  emk::RegularPolygon blue_hex;

  emk::RawImage raw_image;

public:
  KineticExample()
    : stage(578, 200, "container")
    , raw_image("notreal")
    , blue_hex(100, stage.GetHeight()/2, 6, 70, "#00D2FF", "black", 10, true)
  {
    layer.Add(blue_hex);
    stage.Add(layer);
  }

};



KineticExample * example;

extern "C" int emkMain()
{
  example = new KineticExample();

  return 0;
}
