#ifndef EMK_CONTROL_H
#define EMK_CONTROL_H

#include <map>

#include "../libs/Kinetic.h"
#include "Button.h"
#include "ButtonSet.h"
#include "Events.h"
#include "Grid.h"
#include "Panel.h"

namespace emk {

  class Control {
  protected:
    std::map<std::string, Stage *> stage_map;
    std::map<std::string, Layer *> layer_map;

    std::map<std::string, Image *> image_map;
    std::map<std::string, Rect *> rect_map;
    std::map<std::string, Text *> text_map;

    std::map<std::string, Button *> button_map;
    std::map<std::string, ButtonSet *> buttonset_map;
    std::map<std::string, Grid *> grid_map;
    std::map<std::string, Panel *> panel_map;

    std::map<std::string, Callback *> animation_map;  // Animations are templates so using base class in map.
    std::map<std::string, EventChain *> eventchain_map;
    std::map<std::string, Tween *> tween_map;

    std::map<std::string, Shape *> shape_map; // Fill map of all objects, by name.

    Stage * cur_stage;
    Layer * cur_layer;
    Image * cur_image;
    Rect * cur_rect;
    Text * cur_text;
    Button * cur_button;
    ButtonSet * cur_buttonset;
    Grid * cur_grid;
    Panel * cur_panel;
    // cur_animation ??
    EventChain * cur_eventchain;
    Tween * cur_tween;
    
  public:
    Control(int width=1200, int height=800, const std::string & name="container") : cur_layer(NULL) {
      AddStage(name, width, height); // Build the default stage.
      Stage().ResizeMax();
    }
    ~Control() {
      // @CAO We need to delete all objects created as part of the controller.
    }

    Stage & AddStage(const std::string & name, int x, int y) {
      cur_stage = new emk::Stage(x, y, name);
      stage_map[name] = cur_stage;
      return *cur_stage;
    }

    Layer & AddLayer(const std::string & name) {
      cur_layer = new emk::Layer();
      layer_map[name] = cur_layer;
      return *cur_layer;
    }

    Image & AddImage(const std::string & name, const std::string & filename) {
      cur_image = new emk::Image(filename);
      image_map[name] = cur_image;
      return *cur_image;
    }

    Rect & AddRect(const std::string & name, int x=0, int y=0, int w=10, int h=10,
                   std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0) {
      cur_rect = new emk::Rect(x, y, w, h, fill, stroke, stroke_width, draggable);
      rect_map[name] = cur_rect;
      shape_map[name] = cur_rect;
      return *cur_rect;
    }

    Text & AddText(const std::string & name, int x=0, int y=0, std::string text="", int font_size=30, std::string font_family="Calibri", std::string fill="black") {
      cur_text = new emk::Text(x, y, text, font_size, font_family, fill);
      text_map[name] = cur_text;
      shape_map[name] = cur_text;
      return *cur_text;
    }

    template<class T> Button & AddButton(const std::string & name, T * target, void (T::*method_ptr)()) {
      cur_button = new emk::Button(target, method_ptr, name);
      button_map[name] = cur_button;
      shape_map[name] = cur_button;
      return *cur_button;
    }

    ButtonSet & AddButtonSet(const std::string & name, int cols, int rows, int x, int y, int width, int height, int spacing=0) {
      cur_buttonset = new emk::ButtonSet(cols, rows, x, y, width, height, spacing);
      buttonset_map[name] = cur_buttonset;
      return *cur_buttonset;
    }

    Grid & AddGrid(const std::string & name, int x, int y, int width, int height, int cols, int rows, int num_colors=12, int border_width=1) {
      cur_grid = new emk::Grid(x, y, width, height, cols, rows, num_colors, border_width);
      grid_map[name] = cur_grid;
      shape_map[name] = cur_grid;
      return *cur_grid;
    }

    EventChain & AddEventChain(const std::string & name) {
      cur_eventchain = new emk::EventChain();
      eventchain_map[name] = cur_eventchain;
      return *cur_eventchain;
    }

    Tween & AddTween(const std::string & name, Object & target, double seconds) {
      cur_tween = new emk::Tween(target, seconds);
      tween_map[name] = cur_tween;
      return *cur_tween;
    }


    Stage & Stage(const std::string & name="") {
      if (name != "") cur_stage = stage_map[name];
      assert(cur_stage != NULL);
      return *cur_stage;
    }
    Layer & Layer(const std::string & name="") {
      if (name != "") cur_layer = layer_map[name];
      assert(cur_layer != NULL);
      return *cur_layer;
    }
    Image & Image(const std::string & name="") {
      if (name != "") cur_image = image_map[name];
      assert(cur_image != NULL);
      return *cur_image;
    }
    Rect & Rect(const std::string & name="") {
      if (name != "") cur_rect = rect_map[name];
      assert(cur_rect != NULL);
      return *cur_rect;
    }
    Text & Text(const std::string & name="") {
      if (name != "") cur_text = text_map[name];
      assert(cur_text != NULL);
      return *cur_text;
    }
    Button & Button(const std::string & name="") {
      if (name != "") cur_button = button_map[name];
      assert(cur_button != NULL);
      return *cur_button;
    }
    ButtonSet & ButtonSet(const std::string & name="") {
      if (name != "") cur_buttonset = buttonset_map[name];
      assert(cur_buttonset != NULL);
      return *cur_buttonset;
    }
    Grid & Grid(const std::string & name="") {
      if (name != "") cur_grid = grid_map[name];
      assert(cur_grid != NULL);
      return *cur_grid;
    }
    EventChain & EventChain(const std::string & name="") {
      if (name != "") cur_eventchain = eventchain_map[name];
      assert(cur_eventchain != NULL);
      return *cur_eventchain;
    }
    Tween & Tween(const std::string & name="") {
      if (name != "") cur_tween = tween_map[name];
      assert(cur_tween != NULL);
      return *cur_tween;
    }

  };

};

#endif
