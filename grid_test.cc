#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Random.h"
#include "utils/Button.h"
#include "utils/Grid.h"

using namespace std;

class GridExample {
private:
  int cols;
  int rows;
  int num_cells;

  std::vector<double> merits;

  emkStage stage;
  emkLayer layer;

  emkText title;
  emk::Grid grid;
  emk::Random random;
  emk::Button pause_button;

  emkAnimation<GridExample> anim;

  emk::ProbSchedule sched;

  int next_id;
  bool color2;
  int frame_count;
  bool pause;
public:
  GridExample(int _cols, int _rows)
    : cols(_cols), rows(_rows), num_cells(cols*rows), merits(num_cells)
    , stage(1200, 800, "container")
    , title(10, 10, "Grid viewer test!", "30", "Calibri", "black")
    , grid(50, 50, 600, 600, cols, rows, 60)
    , pause_button(this, &GridExample::PauseButton)
    , sched(num_cells)
    , frame_count(0)
    , pause(false)
  {
    layer.Add(grid);
    layer.Add(title);
    stage.Add(layer);

    pause_button.SetLayout(610, 0, 30, 30);

    for (int i = 0; i < num_cells; i++) {
      grid.SetColor(i, random.GetInt(60));
      merits[i] = random.GetDouble();
      sched.Adjust(i, merits[i]);
    }

    anim.Setup(this, &GridExample::Animate, layer);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    frame_count++;
    for (int i = 0; i < 100; i++) {
      // int from = random.GetInt(num_cells);
      int from = sched.NextID();
      int to = from + (random.GetInt(3) - 1) + (random.GetInt(3) - 1) * 60;
      if (from == to) continue;
      if (to >= num_cells) to -= num_cells;
      if (to < 0) to += num_cells;
      grid.SetColor(to, grid.GetColor(from));
      sched.Adjust(to, merits[from]);
      merits[to] = merits[from];
    }
  }

  void PauseButton() {
    if (pause == true) { anim.Start(); pause = false; }
    else { anim.Stop(); pause = true; }
  }
};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample(60, 60);

  return 0;
}
