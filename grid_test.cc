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
  emkLayer layer_static;     // Background layer that should never need to be updated.
  emkLayer layer_grid;       // Main layer for the grid and anything that gets updated with it.
  emkLayer layer_gridmouse;  // Fast updating as the mouse is moved over the grid.
  emkLayer layer_info;       // Layer to print the side information.
  emkLayer layer_buttons;    // Layer for all of the buttons on the screen.

  emkText title;
  emk::Grid grid;
  emk::Random random;
  emk::Button button_pause;
  emkText update_text;       // On main layer
  emkText mouse_text;        // On gridmouse layer
  emkText click_text;        // On info layer

  emkAnimation<GridExample> anim;


  // Current status
  emk::ProbSchedule sched;
  int update;
  bool pause;
public:
  GridExample(int _cols, int _rows)
    : cols(_cols), rows(_rows), num_cells(cols*rows), merits(num_cells)
    , stage(1200, 800, "container")
    , title(10, 10, "Grid viewer test!", "30", "Calibri", "black")
    , grid(50, 50, 601, 601, cols, rows, 60)
    , button_pause(this, &GridExample::PauseButton)
    , update_text(670, 50, "Update: ", "30", "Calibri", "black")
    , mouse_text(670, 90, "Move mouse over grid to test!", "30", "Calibri", "black")
    , click_text(670, 130, "Click on grid to test!", "30", "Calibri", "black")
    , sched(num_cells)
    , update(0), pause(false)
  {
    button_pause.SetLayout(335, 655, 30, 30);
    button_pause.SetupDrawIcon(this, &GridExample::DrawPauseButton);

    grid.SetMouseMoveCallback(this, &GridExample::Draw_Gridmouse);
    grid.SetClickCallback(this, &GridExample::Draw_Gridclick);

    layer_static.Add(title);
    layer_grid.Add(grid);
    layer_grid.Add(update_text);
    layer_gridmouse.Add(grid.GetMousePointer());
    layer_gridmouse.Add(mouse_text);
    layer_info.Add(click_text);
    layer_buttons.Add(button_pause);

    stage.Add(layer_static);
    stage.Add(layer_grid);
    stage.Add(layer_gridmouse);
    stage.Add(layer_info);
    stage.Add(layer_buttons);

    for (int i = 0; i < num_cells; i++) {
      grid.SetColor(i, random.GetInt(60));
      merits[i] = random.GetDouble();
      sched.Adjust(i, merits[i]);
    }

    anim.Setup(this, &GridExample::Animate, layer_grid);
    anim.Start();
  }

  void Animate(const emkAnimationFrame & frame) {
    update++;
    update_text.SetText(std::string("Update: ") + std::to_string(update));
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

  void Draw_Gridmouse() {
    mouse_text.SetText(std::string("Mouse Col:") + std::to_string(grid.GetMouseCol()) + std::string(" Row:") + std::to_string(grid.GetMouseRow()));
  }

  void Draw_Gridclick() {
    click_text.SetText(std::string("Click Col:") + std::to_string(grid.GetClickCol()) + std::string(" Row:") + std::to_string(grid.GetClickRow()));
    layer_info.BatchDraw();
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
