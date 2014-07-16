#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Random.h"
#include "utils/Button.h"
#include "utils/Grid.h"

class GridExample {
private:
  int cols;
  int rows;
  int num_cells;

  int grid_x;
  int grid_y;
  int grid_w;
  int grid_h;

  int grid_offset_x;
  int grid_offset_y;

  std::vector<double> merits;

  emk::Stage stage;
  emk::Layer layer_static;     // Background layer that should never need to be updated.
  emk::Layer layer_grid;       // Main layer for the grid and anything that gets updated with it.
  emk::Layer layer_gridmouse;  // Fast updating as the mouse is moved over the grid.
  emk::Layer layer_info;       // Layer to print the side information.
  emk::Layer layer_buttons;    // Layer for all of the buttons on the screen.

  emk::Text title;
  emk::Grid grid;             // Visual Grid.
  emk::Button button_rewind;  // BUTTON: Restart a run.
  emk::Button button_pause;   // BUTTON: Pause a run.
  emk::Button button_freeze;  // BUTTON: Save a population.
  emk::Button button_config;  // BUTTON: Save a population.
  emk::Image image_icon_config; // Image for config button

  emk::Text update_text;       // On main layer
  emk::Text mouse_text;        // On gridmouse layer
  emk::Text click_text;        // On info layer

  emk::Animation<GridExample> anim_grid_run;

  emk::Tween tween_grid_flip;

  // Current status 
  emk::Random random;        // Random number generator
  emk::ProbSchedule sched;
  int update;
  bool is_paused;            // false = grid running.  true = grid paused.
  bool is_flipped;           // false = grid showing.  true = config showing.
public:
  GridExample(int _cols, int _rows)
    : cols(_cols), rows(_rows), num_cells(cols*rows)
    , grid_x(50), grid_y(50), grid_w(481), grid_h(481)
    , grid_offset_x(grid_x + grid_w/2), grid_offset_y(grid_y + grid_h/2)
    , merits(num_cells)
    , stage(1200, 800, "container")
    , title(10, 10, "Avida Viewer test!", "30", "Calibri", "black")
    , grid(grid_offset_x, grid_offset_y, grid_w, grid_h, cols, rows, 60)
    , button_rewind(this, &GridExample::SetupRun)
    , button_pause(this, &GridExample::PauseRun)
    , button_freeze(this, &GridExample::FreezeRun)
    , button_config(this, &GridExample::ConfigRun)
    , image_icon_config("icons/gear.png") // ("icons/setting.png")
    , update_text(670, 50, "Update: ", "30", "Calibri", "black")
    , mouse_text(670, 90, "Move mouse over grid to test!", "30", "Calibri", "black")
    , click_text(670, 130, "Click on grid to test!", "30", "Calibri", "black")
    , tween_grid_flip(update_text, 3)
    , sched(num_cells)
    , update(0), is_paused(false), is_flipped(false)
  {
    // Setup the buttons a long the bottom of the grid.
    const int buttons_x = grid_x;
    const int buttons_y = grid_y + grid_h + 5;
    const int button_w = 40;
    const int button_spacing = 5;
    button_rewind.SetLayout(buttons_x + grid_w/2 - button_w * 1.5 - button_spacing, buttons_y, button_w, button_w);
    button_rewind.SetupDrawIcon(this, &GridExample::DrawRewindButton);
    button_pause.SetLayout(buttons_x + (grid_w - button_w)/2, buttons_y, button_w, button_w);
    button_pause.SetupDrawIcon(this, &GridExample::DrawPauseButton);
    button_freeze.SetLayout(buttons_x + (grid_w + button_w)/2 + button_spacing, buttons_y, button_w, button_w);
    button_freeze.SetupDrawIcon(this, &GridExample::DrawFreezeButton);
    button_config.SetLayout(buttons_x + grid_w - button_w, buttons_y, button_w, button_w);
    button_config.SetupDrawIcon(this, &GridExample::DrawConfigButton);
    // button_config.SetFillPatternImage(image_icon_config);

    grid.SetMouseMoveCallback(this, &GridExample::Draw_Gridmouse);
    grid.SetClickCallback(this, &GridExample::Draw_Gridclick);
    grid.SetOffset(grid.GetWidth()/2, grid.GetHeight()/2);

    layer_static.Add(title);
    layer_grid.Add(grid);
    layer_grid.Add(update_text);
    layer_gridmouse.Add(grid.GetMousePointer());
    layer_gridmouse.Add(mouse_text);
    layer_info.Add(click_text);
    layer_buttons.Add(button_rewind);
    layer_buttons.Add(button_pause);
    layer_buttons.Add(button_freeze);
    layer_buttons.Add(button_config);

    stage.Add(layer_static);
    stage.Add(layer_grid);
    stage.Add(layer_gridmouse);
    stage.Add(layer_info);
    stage.Add(layer_buttons);

    SetupRun();

    // Setup potential animations...
    anim_grid_run.Setup(this, &GridExample::Animate_Grid, layer_grid);

    tween_grid_flip.SetXY(800, 550);

    // And start the main animation
    anim_grid_run.Start();
  }

  void SetupRun() {
    update = 0;
    for (int i = 0; i < num_cells; i++) {
      grid.SetColor(i, random.GetInt(0));
      merits[i] = 0.0;
      sched.Adjust(i, 0.0);
      // grid.SetColor(i, random.GetInt(60));
      // merits[i] = random.GetDouble();
      // sched.Adjust(i, merits[i]);
    }
    grid.SetColor(1830, 40);
      merits[1830] = 1.0;
      sched.Adjust(1830, 1.0);

    layer_grid.BatchDraw();
  }

  void PauseRun() {
    if (is_paused == true) { anim_grid_run.Start(); is_paused = false; }
    else { anim_grid_run.Stop(); is_paused = true; }
  }

  void FreezeRun() {
    emk::Alert("Sorry, freezing not implemented yet!");
  }

  void ConfigRun() {  // Other side of grid is config.
    /*
    // If we're on the config side...
    if (is_flipped) {
      // Restart the grid if it's not paused.
      if (is_paused == false) anim_grid_run.Start();
      is_flipped = false;
    }

    // If we're on the grid side...
    else {
      // Stop the grid before flipping.
      if (is_paused == false) anim_grid_run.Stop();
      is_flipped = true;
    }
    
    // @CAO Shut off listening on grid!
    anim_grid_flip.Start();
    */

    tween_grid_flip.SetXY(random.GetInt(600)+600, random.GetInt(600));
    tween_grid_flip.Play();
  }

  void Animate_Grid(const emk::AnimationFrame & frame) {
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

  void DrawRewindButton(emk::Canvas & canvas) {
    canvas.SetStroke("#440000");
    canvas.SetFillStyle("#440000");

    
    // Left bar...
    canvas.Rect(0, 20, 15, 60, true);

    // Arrow 1...
    canvas.BeginPath();
    canvas.MoveTo(20, 50);
    canvas.LineTo(60, 20);
    canvas.LineTo(60, 80);
    canvas.ClosePath();
    canvas.Fill();

    // Arrow 2...
    canvas.BeginPath();
    canvas.MoveTo(55, 50);
    canvas.LineTo(95, 20);
    canvas.LineTo(95, 80);
    canvas.ClosePath();
    canvas.Fill();

    canvas.Stroke();
  }

  void DrawPauseButton(emk::Canvas & canvas) {
    canvas.SetStroke("black");
    canvas.SetFillStyle("black");

    // Draw a play sign if paused or a pause sign if running.
    if (is_paused) {
      canvas.BeginPath();
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

  void DrawFreezeButton(emk::Canvas & canvas) {
    canvas.SetStroke("#222288");
    canvas.SetFillStyle("#222288");

    // Draw a snowflake.
    canvas.Translate(50, 50);
    for (int i = 0; i < 3; i++) {
      canvas.Rect(-8, -50, 16, 100, true);
      canvas.Rotate(emk::PI/3);
    }

    canvas.Stroke();
  }

  void DrawConfigButton(emk::Canvas & canvas) {
    // canvas.SetStroke("#008800");
    // canvas.SetFillStyle("#008800");
    canvas.SetStroke("#708090");
    canvas.SetFillStyle("#708090");

    // Draw Gear 1
    canvas.Translate(35, 35);
    canvas.SetLineWidth(10);
    canvas.BeginPath();
    canvas.Arc(0, 0, 30, 0, 2.0*emk::PI);
    canvas.Stroke();

    for (int i=0; i < 12; i++) {
      canvas.Rect(-5, 30, 10, 15, true);
      canvas.Rotate(emk::PI/6);
    }

    // Draw Gear 2
    canvas.Translate(45, 40);
    canvas.SetLineWidth(10);
    canvas.BeginPath();
    canvas.Arc(0, 0, 20, 0, 2.0*emk::PI);
    canvas.Stroke();

    for (int i=0; i < 8; i++) {
      canvas.Rect(-5, 20, 10, 12, true);
      canvas.Rotate(emk::PI/4);
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
