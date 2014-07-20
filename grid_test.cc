#include <emscripten.h>
#include <iostream>
#include <cmath>

#include "Kinetic.h"
#include "tools/Random.h"
#include "utils/Button.h"
#include "utils/Events.h"
#include "utils/Grid.h"

class GridExample {
private:
  int cols;
  int rows;
  int num_cells;
  int num_colors;

  int logo_x;  int logo_y;  int logo_w;  int logo_h;
  int grid_x;  int grid_y;  int grid_w;  int grid_h;

  std::vector<double> merits;

  emk::Stage stage;
  emk::Layer layer_static;     // Background layer that should never need to be updated.
  emk::Layer layer_grid;       // Main layer for the grid and anything that gets updated with it.
  emk::Layer layer_gridmouse;  // Fast updating as the mouse is moved over the grid.
  emk::Layer layer_info;       // Layer to print the side information.
  emk::Layer layer_buttons;    // Layer for all of the buttons on the screen.

  emk::Text title;

  emk::Image image_avida_logo; // Image for Avida Logo
  emk::Rect rect_avida_logo;   // Image holder for Avida Logo

  emk::Button button_mode_population; // Population mode
  emk::Button button_mode_organism;   // Population mode
  emk::Button button_mode_analysis;   // Population mode

  emk::Grid grid;              // Visual Grid.
  emk::Button button_rewind;   // BUTTON: Restart a run.
  emk::Button button_pause;    // BUTTON: Pause a run.
  emk::Button button_freeze;   // BUTTON: Save a population.
  emk::Button button_config;   // BUTTON: Save a population.

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

  double mut_rate;
public:
  GridExample(int _cols, int _rows, int _colors)
    : cols(_cols), rows(_rows), num_cells(cols*rows), num_colors(_colors)
    , logo_x(5), logo_y(10), logo_w(130), logo_h(logo_w*181/205)  
    , grid_x(logo_x + logo_w + 10), grid_y(10), grid_w(481), grid_h(481)
    , merits(num_cells)
    , stage(1200, 800, "container")
    , title(650, 10, "Avida Viewer test!", "30", "Calibri", "black")
    , image_avida_logo("icons/avidalogo.jpg") // ("icons/setting.png")
    , rect_avida_logo(logo_x, logo_y, logo_w, logo_h, "white", "black", 4)
    , button_mode_population(this, &GridExample::ModePopulation)
    , button_mode_organism(this, &GridExample::ModeOrganism)
    , button_mode_analysis(this, &GridExample::ModeAnalysis)
    , grid(grid_x, grid_y, grid_w, grid_h, cols, rows, num_colors+1)
    , button_rewind(this, &GridExample::SetupRun)
    , button_pause(this, &GridExample::PauseRun)
    , button_freeze(this, &GridExample::FreezeRun)
    , button_config(this, &GridExample::ConfigRun)
    , update_text(650, 50, "Update: ", "30", "Calibri", "black")
    , mouse_text(650, 90, "Move mouse over grid to test!", "30", "Calibri", "black")
    , click_text(650, 130, "Click on grid to test!", "30", "Calibri", "black")
    , tween_grid_flip(grid, 1)
    , sched(num_cells)
    , update(0), is_paused(false), is_flipped(false)
    , mut_rate(0.01)
  {
    rect_avida_logo.SetFillPatternImage(image_avida_logo);
    rect_avida_logo.SetFillPatternScale( ((double) logo_w) / 205.0  );
    // emk::Alert(image_avida_logo.GetWidth());

    const int mode_x = rect_avida_logo.GetX();
    const int mode_y = rect_avida_logo.GetY() + rect_avida_logo.GetHeight() + 10;
    const int mode_w = rect_avida_logo.GetWidth();
    const int mode_h = 40;
    button_mode_population.SetLayout(mode_x, mode_y, mode_w, mode_h);
    button_mode_population.SetBGColor("white");
    button_mode_population.SetRoundCorners(true, false, false, true);
    button_mode_population.SetupDrawIcon(this, &GridExample::DrawPopulationModeButton);

    button_mode_organism.SetLayout(mode_x, mode_y+mode_h+5, mode_w, mode_h);
    button_mode_organism.SetBGColor("white");
    button_mode_organism.SetRoundCorners(true, false, false, true);
    button_mode_organism.SetupDrawIcon(this, &GridExample::DrawOrganismModeButton);

    button_mode_analysis.SetLayout(mode_x, mode_y+2*mode_h+10, mode_w, mode_h);
    button_mode_analysis.SetBGColor("white");
    button_mode_analysis.SetRoundCorners(true, false, false, true);
    button_mode_analysis.SetupDrawIcon(this, &GridExample::DrawAnalysisModeButton);
 
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

    // @CAO TESTING!  Fix this!!
    emk::Grid * grid_spect = new emk::Grid(grid_x, buttons_y+button_w+10, grid_w, grid_w/60, 60, 1, 60);
    for (int i = 0; i < 61; i++) grid_spect->SetColor(i, i);
    layer_static.Add(*grid_spect);



    grid.SetMouseMoveCallback(this, &GridExample::Draw_Gridmouse);
    grid.SetClickCallback(this, &GridExample::Draw_Gridclick);

    layer_static.Add(title);
    layer_static.Add(rect_avida_logo);
    layer_grid.Add(grid);
    layer_grid.Add(update_text);
    layer_gridmouse.Add(grid.GetMousePointer());
    layer_gridmouse.Add(mouse_text);
    layer_info.Add(click_text);
    layer_buttons.Add(button_mode_population);
    layer_buttons.Add(button_mode_organism);
    layer_buttons.Add(button_mode_analysis);
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

    // And start the main animation
    anim_grid_run.Start();
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
    // If we're on the config side...
    if (is_flipped) {
      tween_grid_flip.SetXY(grid_x, grid_y);
      tween_grid_flip.SetScaleXY(1.0, 1.0);
      grid.GetMousePointer().SetVisible(true);
      tween_grid_flip.Play();

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

      tween_grid_flip.SetXY(grid_x+grid_w/2, grid_y);
      tween_grid_flip.SetScaleXY(0.0, 1.0);
      grid.GetMousePointer().SetVisible(false);
      grid.GetMousePointer().DrawLayer();
      
      tween_grid_flip.Play();
    }
    
    // @CAO Shut off listening on grid!
    // anim_grid_flip.Start();

    // static bool status = false;
    // status = !status;
    // if (status) grid.SetListening(false);
    // else grid.SetListening(true);
  }

  void Animate_Grid(const emk::AnimationFrame & frame) {
    update++;
    update_text.SetText(std::string("Update: ") + std::to_string(update));
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
    mouse_text.SetText(std::string("Mouse Col:") + std::to_string(grid.GetMouseCol()) + std::string(" Row:") + std::to_string(grid.GetMouseRow()));
  }

  void Draw_Gridclick() {
    click_text.SetText(std::string("Click Col:") + std::to_string(grid.GetClickCol()) + std::string(" Row:") + std::to_string(grid.GetClickRow()));
    layer_info.BatchDraw();
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
  example = new GridExample(60, 60, 60);

  return 0;
}
