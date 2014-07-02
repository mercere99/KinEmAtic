#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Grid.h"

using namespace std;

class GridExample {
private:
  emkStage stage;
  emkLayer layer;

  emkText title;
  emk::Grid grid;

  emkAnimation<GridExample> anim;

  int next_id;
  bool color2;
public:
  GridExample()
    : stage(1200, 800, "container")
    , title(10, 10, "Grid viewer test!", "30", "Calibri", "black")
    , grid(50, 50, 600, 600, 60, 60)
  {
    layer.Add(grid);
    layer.Add(title);
    stage.Add(layer);

    anim.Setup(this, &GridExample::Animate, layer);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    if (next_id > grid.GetNumCells()) { next_id = 0; color2 = !color2; }
    grid.SetColor(next_id++, color2 ? 2 : 3);
  }
};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample();

  return 0;
}
