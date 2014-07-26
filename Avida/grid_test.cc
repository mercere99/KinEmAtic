#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "../libs/Kinetic.h"
#include "../tools/Random.h"
#include "../tools/debug.h"
#include "../cogs/Button.h"
#include "../cogs/ButtonSet.h"
#include "../cogs/Events.h"
#include "../cogs/Grid.h"
#include "../cogs/Panel.h"

#include "../cogs/Control.h"

#define EMK_DEBUG // Turn on DEBUG mode...

class GridExample {
private:
  int cols;
  int rows;
  int num_cells;
  int num_colors;

  int logo_x;  int logo_y;  int logo_w;  int logo_h;
  int grid_x;  int grid_y;  int grid_w;  int grid_h;

  std::vector<double> merits;

  emk::Control control;           // Main controller for GUI elements

  emk::Image image_avida_logo; // Image for Avida Logo
  emk::Rect rect_avida_logo;   // Image holder for Avida Logo

  emk::Grid grid;              // Visual Grid.
  emk::Panel panel_config;     // Congifuration options.

  emk::Animation<GridExample> anim_grid_run;

  emk::EventChain event_grid_flip;

  // Current status 
  emk::Random random;        // Random number generator
  emk::ProbSchedule sched;
  int update;
  bool is_paused;            // false = grid running.  true = grid paused.
  bool is_flipped;           // false = grid showing.  true = config showing.

  double mut_rate;
public:
  GridExample(int _cols, int _rows, int _colors)
    : cols(_cols), rows(_rows), num_cells(cols*rows), num_colors(_colors)
    , logo_x(5), logo_y(10), logo_w(130), logo_h(logo_w*181/205)  
    , grid_x(logo_x + logo_w + 10), grid_y(10), grid_w(481), grid_h(481)
    , merits(num_cells)
      //    , stage(1200, 800, "container")
    , image_avida_logo("../icons/avidalogo.jpg") // ("icons/setting.png")
    , rect_avida_logo(logo_x, logo_y, logo_w, logo_h, "white", "black", 4)
    , grid(grid_x, grid_y, grid_w, grid_h, cols, rows, num_colors+1)
    , panel_config(grid_x+grid_w/2, grid_y, grid_w, grid_h)
    , sched(num_cells)
    , update(0), is_paused(true), is_flipped(false)
    , mut_rate(0.01)
  {
    emk::Alert("Testing!");
    assert(false);

    // Setup Avida Logo
    rect_avida_logo.SetFillPatternImage(image_avida_logo);
    rect_avida_logo.SetFillPatternScale( ((double) logo_w) / 205.0  );


    // Setup the mode buttons
    emk::ButtonSet & mode_buttons =
      control.AddButtonSet("modes", 1, 4, rect_avida_logo.GetX(), rect_avida_logo.GetY() + rect_avida_logo.GetHeight() + 10,
                           rect_avida_logo.GetWidth(), 40, 5);

    mode_buttons[0].SetTrigger(this, &GridExample::ModePopulation);  // Population mode
    mode_buttons[1].SetTrigger(this, &GridExample::ModeOrganism);    // Organism mode
    mode_buttons[2].SetTrigger(this, &GridExample::ModeAnalysis);    // Analysis mode
    mode_buttons[3].SetTrigger(this, &GridExample::ModeConfigure);    // Analysis mode

    mode_buttons[0].SetDrawIcon(this, &GridExample::DrawPopulationModeButton);
    mode_buttons[1].SetDrawIcon(this, &GridExample::DrawOrganismModeButton);
    mode_buttons[2].SetDrawIcon(this, &GridExample::DrawAnalysisModeButton);
    mode_buttons[3].SetDrawIcon(this, &GridExample::DrawConfigModeButton);

    mode_buttons.SetBGColor("white");
    mode_buttons.SetRoundCorners(true, false, false, true);

 
    // Setup the buttons centered along the bottom of the grid.

    const int num_buttons = 3;
    const int button_w = 40;
    const int button_spacing = 5;
    const int button_set_w = button_w * num_buttons + button_spacing * (num_buttons-1);
    const int buttons_x = grid_x + (grid_w - button_set_w)/2;
    emk::ButtonSet & grid_buttons = control.AddButtonSet("grid", 3, 1, buttons_x, grid_y + grid_h + 5, button_w, button_w, button_spacing);

    grid_buttons[0].SetTrigger(this, &GridExample::SetupRun);
    grid_buttons[1].SetTrigger(this, &GridExample::PauseRun);
    grid_buttons[2].SetTrigger(this, &GridExample::FreezeRun);

    grid_buttons[0].SetDrawIcon(this, &GridExample::DrawRewindButton);
    grid_buttons[1].SetDrawIcon(this, &GridExample::DrawPauseButton);
    grid_buttons[2].SetDrawIcon(this, &GridExample::DrawFreezeButton);


    // Setup the text.
    emk::Text & text_title  = control.AddText("title", 650, 10, "Avida Viewer test!", "30", "Calibri", "black");
    emk::Text & text_update = control.AddText("update", 650, 50, "Update: 0", "30", "Calibri", "black");
    emk::Text & text_mouse  = control.AddText("mouse", 650, 90, "Move mouse over grid to test!", "30", "Calibri", "black");
    emk::Text & text_click  = control.AddText("click", 650, 130, "Click on grid to test!", "30", "Calibri", "black");


    // Create all of the layers
    emk::Layer & layer_static = control.AddLayer("static");       // Background layer that should never need to be updated.
    emk::Layer & layer_grid = control.AddLayer("grid");           // Main layer for the grid and anything updated with it.
    emk::Layer & layer_gridmouse = control.AddLayer("gridmouse"); // Fast updating as the mouse is moved over the grid.
    emk::Layer & layer_info = control.AddLayer("info");           // Layer to print the side information.
    emk::Layer & layer_buttons = control.AddLayer("buttons");     // Layer for all of the buttons on the screen.




    // Build a range of colors below the main grid.
    emk::Grid & grid_spect = control.AddGrid("spect", grid_x, grid_y+grid_h+button_w+15, grid_w, grid_w/60, 60, 1, 60);
    for (int i = 0; i < 61; i++) grid_spect.SetColor(i, i);
    layer_static.Add(grid_spect);


    grid.SetMouseMoveCallback(this, &GridExample::Draw_Gridmouse);
    grid.SetClickCallback(this, &GridExample::Draw_Gridclick);

    panel_config.SetScale(0.0, 1.0);

    layer_static.Add(text_title);
    layer_static.Add(rect_avida_logo);
    layer_grid.Add(grid);
    layer_grid.Add(panel_config);
    layer_grid.Add(text_update);
    layer_gridmouse.Add(grid.GetMousePointer());
    layer_gridmouse.Add(text_mouse);
    layer_info.Add(text_click);
    layer_buttons.Add(mode_buttons);
    layer_buttons.Add(grid_buttons);

    emk::Stage & stage = control.Stage();
    stage.Add(layer_static);
    stage.Add(layer_grid);
    stage.Add(layer_gridmouse);
    stage.Add(layer_info);
    stage.Add(layer_buttons);

    SetupRun();

    // Setup potential animations...
    control.AddTween("grid_flip", grid, 0.5);
    control.AddTween("panel_flip", panel_config, 0.5);
    anim_grid_run.Setup(this, &GridExample::Animate_Grid, layer_grid);
  }

  void ModePopulation() {
    // Nothing here yet...
  }

  void ModeOrganism() {
    // Nothing here yet...
  }

  void ModeAnalysis() {
    // Nothing here yet...
  }

  void ModeConfigure() {  // Other side of grid is config.
    // If we're on the config side...
    if (is_flipped) {
      // Stop the config panel from listening for inputs.
      panel_config.SetListening(false);

      // Setup the animation
      emk::Tween & part1 = control.Tween("panel_flip").SetX(grid_x+grid_w/2).SetScaleX(0.0);
      emk::Tween & part2 = control.Tween("grid_flip").SetX(grid_x).SetScaleX(1.0);
      grid.GetMousePointer().SetVisible(true);
      event_grid_flip.First(part1).Then(part2).Trigger();

      // Restart the grid if it's not paused.
      if (is_paused == false) anim_grid_run.Start();
      is_flipped = false;
      grid.SetListening(true);
    }

    // If we're on the grid side...
    else {
      // Stop the grid before flipping.
      grid.SetListening(false);
      if (is_paused == false) anim_grid_run.Stop();
      is_flipped = true;
      
      emk::Tween & part1 = control.Tween("grid_flip").SetX(grid_x+grid_w/2).SetScaleX(0.0);
      emk::Tween & part2 = control.Tween("panel_flip").SetX(grid_x).SetScaleX(1.0);
      grid.GetMousePointer().SetVisible(false);
      grid.GetMousePointer().DrawLayer();
      
      event_grid_flip.First(part1).Then(part2).Trigger();
      panel_config.SetListening(true);
    }
    
  }

  void SetupRun() {
    update = 0;
    for (int i = 0; i < num_cells; i++) {
      grid.SetColor(i, 0);
      merits[i] = 0.0;
      sched.Adjust(i, 0.0);
      // grid.SetColor(i, random.GetInt(num_colors)+1);
      // merits[i] = random.GetDouble();
      // sched.Adjust(i, merits[i]);
    }
    grid.SetColor(1830, 1);
    merits[1830] = 1.0;
    sched.Adjust(1830, 1.0);

    control.Layer("grid").BatchDraw();
  }

  void PauseRun() {
    if (is_paused == true) { anim_grid_run.Start(); is_paused = false; }
    else { anim_grid_run.Stop(); is_paused = true; }
  }

  void FreezeRun() {
    emk::Alert("Sorry, freezing not implemented yet!");
  }

  void Animate_Grid(const emk::AnimationFrame & frame) {
    update++;
    control.Text("update").SetText(std::string("Update: ") + std::to_string(update));
    for (int i = 0; i < 100; i++) {
      int from = sched.NextID();
      int to = from + (random.GetInt(3) - 1) + (random.GetInt(3) - 1) * 60;
      if (from == to) continue;
      if (to >= num_cells) to -= num_cells;
      if (to < 0) to += num_cells;

      if (random.P(mut_rate)) {
        grid.SetColor(to, random.GetInt(num_colors)+1);
        merits[to] = merits[from];
        double quality = random.GetDouble();
        if (quality > 0.99) merits[to] *= 2.0;
        else if (quality < 0.5) merits[to] *= 0.5;
      }
      else {
        grid.SetColor(to, grid.GetColor(from));
        merits[to] = merits[from];
      }
      sched.Adjust(to, merits[to]);

    }
  }



  void Draw_Gridmouse() {
    control.Text("mouse").SetText(std::string("Mouse Col:") + std::to_string(grid.GetMouseCol()) + std::string(" Row:") + std::to_string(grid.GetMouseRow()));
  }

  void Draw_Gridclick() {
    control.Text("click").SetText(std::string("Click Col:") + std::to_string(grid.GetClickCol()) + std::string(" Row:") + std::to_string(grid.GetClickRow()));
    control.Layer("info").BatchDraw();
  }

  void DrawPopulationModeButton(emk::Canvas & canvas) {
    const int offset = 5;
    const int width_cells = 4;
    const int cell_width = (100-2*offset)/width_cells;
    const int grid_width = cell_width * width_cells;

    canvas.SetFillStyle("yellow");
    canvas.Rect(offset + 0*cell_width, offset + 0*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 1*cell_width, offset + 0*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 2*cell_width, offset + 0*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 0*cell_width, offset + 1*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 1*cell_width, offset + 1*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 1*cell_width, offset + 2*cell_width, cell_width, cell_width, true);

    canvas.SetFillStyle("#8000FF");
    canvas.Rect(offset + 3*cell_width, offset + 0*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 2*cell_width, offset + 1*cell_width, cell_width, cell_width, true);

    canvas.SetFillStyle("#4444FF");
    canvas.Rect(offset + 0*cell_width, offset + 2*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 0*cell_width, offset + 3*cell_width, cell_width, cell_width, true);

    canvas.SetFillStyle("orange");
    canvas.Rect(offset + 3*cell_width, offset + 1*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 2*cell_width, offset + 2*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 3*cell_width, offset + 2*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 1*cell_width, offset + 3*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 2*cell_width, offset + 3*cell_width, cell_width, cell_width, true);
    canvas.Rect(offset + 3*cell_width, offset + 3*cell_width, cell_width, cell_width, true);

    // Inner lines
    canvas.SetStroke("#000000");
    canvas.Rect(offset + 1*cell_width, offset + 0*cell_width, cell_width, grid_width, false);
    canvas.Rect(offset + 3*cell_width, offset + 0*cell_width, cell_width, grid_width, false);
    canvas.Rect(offset + 0*cell_width, offset + 1*cell_width, grid_width, cell_width, false);
    canvas.Rect(offset + 0*cell_width, offset + 3*cell_width, grid_width, cell_width, false);

    // Outer box
    canvas.SetLineWidth(6);
    canvas.Rect(offset, offset, grid_width, grid_width, false);

    canvas.SetFillStyle("black");
    canvas.SetFont("50px Arial");
    canvas.Text("Population", 120, 65);
  }

  void DrawOrganismModeButton(emk::Canvas & canvas) {
    canvas.Translate(50, 50);

    canvas.SetStroke("#000000");
    canvas.SetFillStyle("#008800");
    const int num_circles = 12;
    for (int i = 0; i < num_circles; i++) {
      canvas.BeginPath();
      canvas.Arc(0, 40, 8, 0.0, 2*emk::PI);
      canvas.Fill();
      canvas.Stroke();
      canvas.Rotate(2*emk::PI/num_circles);
    }

    canvas.Translate(-50, -50);

    canvas.SetFillStyle("black");
    canvas.SetFont("50px Arial");
    canvas.Text("Organism", 120, 65);
  }

  void DrawAnalysisModeButton(emk::Canvas & canvas) {
    canvas.SetStroke("#888888");
    canvas.SetFillStyle("white");

    // White background
    canvas.Rect(5, 5, 90, 90, true);

    // Background lines...
    canvas.SetLineWidth(4);
    canvas.Rect(5, 5, 90, 90, false);
    canvas.Rect(23, 5, 18, 90, false);
    canvas.Rect(59, 5, 18, 90, false);
    canvas.Rect(5, 23, 90, 18, false);
    canvas.Rect(5, 59, 90, 18, false);

    // Red Line
    canvas.SetLineWidth(8);
    canvas.SetStroke("red");
    canvas.BeginPath();
    canvas.MoveTo(5, 85);
    canvas.LineTo(50, 45);
    canvas.LineTo(60, 60);
    canvas.LineTo(95, 10);
    canvas.Stroke();

    canvas.SetFillStyle("black");
    canvas.SetFont("50px Arial");
    canvas.Text("Analysis", 120, 65);
  }

  void DrawConfigModeButton(emk::Canvas & canvas) {
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

    canvas.SetFillStyle("black");
    canvas.SetFont("50px Arial");
    canvas.Text("Configure", 40, -10);
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
      // @CAO Should draw side-spurs on snowflake so it looks more identifiable.
      canvas.Rotate(emk::PI/3);
    }

    canvas.Stroke();
  }

};



GridExample * example;

extern "C" int emkMain()
{
  example = new GridExample(60, 60, 60);

  return 0;
}
