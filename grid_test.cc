#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Random.h"
#include "utils/Grid.h"

using namespace std;

class GridExample {
private:
  int cols;
  int rows;
  int num_cells;

  emkStage stage;
  emkLayer layer;

  emkText title;
  emk::Grid grid;
  emk::Random random;

  emkAnimation<GridExample> anim;

  emk::ProbSchedule sched;

  int next_id;
  bool color2;
public:
  GridExample(int _cols, int _rows)
    : cols(_cols), rows(_rows), num_cells(cols*rows)
    , stage(1200, 800, "container")
    , title(10, 10, "Grid viewer test!", "30", "Calibri", "black")
    , grid(50, 50, 600, 600, cols, rows, 60)
    , sched(num_cells)
  {
    layer.Add(grid);
    layer.Add(title);
    stage.Add(layer);

    for (int i = 0; i < num_cells; i++) grid.SetColor(i, random.GetInt(60));

    anim.Setup(this, &GridExample::Animate, layer);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    for (int i = 0; i < 100; i++) {
      int id = random.GetInt(num_cells);
      int from = id + (random.GetInt(3) - 1) + (random.GetInt(3) - 1) * 60;
      if (from >= num_cells) from -= num_cells;
      grid.SetColor(id, grid.GetColor(from));
    }
  }
};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample(60, 60);

  return 0;
}
