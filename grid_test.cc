#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Grid.h"

using namespace std;

class KineticExample {
private:
  emkStage stage;
  emkLayer layer;

  emkText title;
  emk::Grid grid;

  emkAnimation<KineticExample> anim;


public:
  KineticExample()
    : stage(1200, 800, "container")
    , grid(50, 50, 600, 600, 60, 60)
    , title(10, 10, "Grid viewer test!", "30", "Calibri", "black")
  {
    layer.Add(grid);
    layer.Add(title);
    stage.Add(layer);

    // anim.Setup(this, &KineticExample::Animate, layer_anim);
    // anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    const double PI = 3.141592653589793238463;
    const double period = 2000.0;
    const double scale = std::sin(frame.time * 2 * PI / period) + 0.001;
  }
};



KineticExample * example;

extern "C" int emkMain()
{
  example = new KineticExample();

  return 0;
}
