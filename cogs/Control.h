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
  private:
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
    std::map<std::string, EventChain *> event_map;
    std::map<std::string, Tween *> tween_map;

    Stage * cur_stage;
    Layer * cur_layer;
    Button * cur_button;
    ButtonSet * cur_buttonset;

  public:
    Control(int width=1200, int height=800, const std::string & name="container") : cur_layer(NULL) {
      AddStage(name, width, height); // Build the default stage.
    }
    ~Control() { ; }

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
    template<class T> Button & AddButton(const std::string & name, T * target, void (T::*method_ptr)()) {
      cur_button = new emk::Button(target, method_ptr, name);
      button_map[name] = cur_button;
      return *cur_button;
    }
    ButtonSet & AddButtonSet(const std::string & name, int cols, int rows, int x, int y, int width, int height, int spacing=0) {
      cur_buttonset = new emk::ButtonSet(cols, rows, x, y, width, height, spacing);
      buttonset_map[name] = cur_buttonset;
      return *cur_buttonset;
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
    Button & Button(const std::string & name="") {
      if (name != "") cur_button = button_map[name];
      assert(cur_button != NULL);
      return *cur_button;
    }
  };

};

#endif
