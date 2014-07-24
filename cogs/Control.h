#ifndef EMK_CONTROL_H
#define EMK_CONTROL_H

#include <map>

#include "../libs/Kinetic.h"

namespace emk {

  class Control {
  private:
    std::map<std::string, Stage *> stage_map;
    std::map<std::string, Layer *> layer_map;

    std::map<std::string, Image *> image_map;
    std::map<std::string, Rect *> rect_map;
    std::map<std::string, Text *> text_map;

    std::map<std::string, Button *> button_map;
    std::map<std::string, Grid *> grid_map;
    std::map<std::string, Panel *> panel_map;

    std::map<std::string, Callback *> animation_map;  // Animations are templates so using base class in map.
    std::map<std::string, EventChain *> event_map;
    std::map<std::string, Tween *> tween_map;

    Stage * cur_stage;
    Layer * cur_layer;

  public:
    Control() : cur_layer(NULL) {
      AddStage(1200, 800, "container"); // Build the default stage.
    }
    ~Control() { ; }

    Stage & AddStage(int x, int y, const std::string & name) {
      cur_stage = new Stage(x, y, name);
      stage_map[name] = cur_stage;
      return *cur_stage;
    }
    Layer & AddLayer(const std::string & name) {
      cur_layer = new Layer();
      layer_map[name] = cur_layer;
      return *cur_layer;
    }


    Stage & Stage(const string & name) {
      cur_stage = stage_map[name];
      return *cur_stage;
    }
    Layer & Layer(const string & name) {
      cur_layer = layer_map[name];
      return *cur_layer;
    }
  };

};

#endif
