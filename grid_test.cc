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
  emkLayer layer_highlight;

  emkText title;
  emk::Grid grid;
  emk::Random random;
  emk::Button pause_button;
  emkText side_text;

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
    , grid(50, 50, 601, 601, cols, rows, 60)
    , pause_button(this, &GridExample::PauseButton)
    , side_text(670, 50, "Side Test!", "30", "Calibri", "black")
    , sched(num_cells)
    , frame_count(0)
    , pause(false)
  {
    pause_button.SetLayout(335, 655, 30, 30);
    pause_button.SetupDrawIcon(this, &GridExample::DrawPauseButton);

    layer.Add(grid);
    layer.Add(title);
    layer.Add(pause_button);
    layer.Add(side_text);
    layer_highlight.Add(grid.GetMousePointer());
    stage.Add(layer);
    stage.Add(layer_highlight);

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

    side_text.SetText(std::string("Mouse Col:") + std::to_string(grid.GetMouseCol()) + std::string(" Row:") + std::to_string(grid.GetMouseRow()));
  }

  void PauseButton() {
    if (pause == true) { anim.Start(); pause = false; }
    else { anim.Stop(); pause = true; }
  }

  void DrawPauseButton(emkCanvas & canvas) {
    canvas.SetStroke("black");
    canvas.SetFillStyle("black");

    if (pause == true) {
      canvas.MoveTo(20, 10);
      canvas.LineTo(20, 90);
      canvas.LineTo(95, 50);
      canvas.ClosePath();
      canvas.Fill();
    }
    else {
      canvas.Rect(20, 5, 25, 90, true);
      canvas.Rect(60, 5, 25, 90, true);
    }

    canvas.Stroke();
  }
};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample(60, 60);

  return 0;
}
