#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Grid.h"
#include "tools/Random.h"

using namespace std;

class GridExample {
private:
  emkStage stage;
  emkLayer layer;

  emkText title;
  emk::Grid grid;
  emk::Random random;

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

    for (int i = 0; i < 3600; i++) grid.SetColor(i, random.GetInt(60));

    anim.Setup(this, &GridExample::Animate, layer);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    for (int i = 0; i < 100; i++) {
      int id = random.GetInt(3600);
      int from = id + (random.GetInt(3) - 1) + (random.GetInt(3) - 1) * 60;
      if (from >= 3600) from -= 3600;
      grid.SetColor(id, grid.GetColor(from));
    }
  }
};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample();

  return 0;
}
