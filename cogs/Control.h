#ifndef EMK_CONTROL_H
#define EMK_CONTROL_H

#include <map>

#include "../tools/Font.h"
#include "../tools/Point.h"
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

    std::map<std::string, Color *> color_map;
    std::map<std::string, Font *> font_map;
    std::map<std::string, Point *> point_map;

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
    Color * cur_color;
    Font * cur_font;
    Point * cur_point;
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
      AddFont("default", emk::Font("Helvetica", 30, "black"));
      AddPoint("default", 10, height/2);
      AddColor("default", "black");
      Stage().ResizeMax();
    }
    ~Control() {
      // @CAO We need to delete all objects created as part of the controller.
    }


    Stage & AddStage(const std::string & name, int x, int y) {
      cur_stage = new emk::Stage(x, y, name);
      if (stage_map.find(name) != stage_map.end()) delete stage_map[name]; // If we a replacing a stage, delete the old one.
      stage_map[name] = cur_stage;
      return *cur_stage;
    }

    Layer & AddLayer(const std::string & name) {
      cur_layer = new emk::Layer();
      if (layer_map.find(name) != layer_map.end()) delete layer_map[name]; // If we a replacing a layer, delete the old one.
      layer_map[name] = cur_layer;
      return *cur_layer;
    }

    Color & AddColor(const std::string & name, const std::string & color_name) {
      cur_color = new emk::Color(color_name);
      if (color_map.find(name) != color_map.end()) delete color_map[name]; // If we a replacing a color, delete the old one.
      color_map[name] = cur_color;
      return *cur_color;
    }

    Font & AddFont(const std::string & name, const emk::Font & font) {
      cur_font = new emk::Font(font);
      if (font_map.find(name) != font_map.end()) delete font_map[name]; // If we a replacing a font, delete the old one.
      font_map[name] = cur_font;
      return *cur_font;
    }

    Point & AddPoint(const std::string & name, int x=0, int y=0) {
      cur_point = new emk::Point(x, y);
      if (point_map.find(name) != point_map.end()) delete point_map[name]; // If we a replacing a point, delete the old one.
      point_map[name] = cur_point;
      return *cur_point;
    }

    Image & AddImage(const std::string & name, const std::string & filename, int x=0, int y=0, int width=0, int height=0) {
      cur_image = new emk::Image(filename, x, y, width, height);
      if (image_map.find(name) != image_map.end()) delete image_map[name]; // If we a replacing a image, delete the old one.
      image_map[name] = cur_image;
      return *cur_image;
    }

    Rect & AddRect(const std::string & name, int x=0, int y=0, int w=10, int h=10,
                   std::string fill="white", std::string stroke="black", int stroke_width=1, int draggable=0) {
      cur_rect = new emk::Rect(x, y, w, h, fill, stroke, stroke_width, draggable);
      if (rect_map.find(name) != rect_map.end()) delete rect_map[name]; // If we a replacing a rect, delete the old one.
      rect_map[name] = cur_rect;
      shape_map[name] = cur_rect;
      return *cur_rect;
    }

    // Text & AddText(const std::string & name, int x=0, int y=0, std::string text="", int font_size=30, std::string font_family="Calibri", std::string fill="black") {
    Text & AddText(const std::string & name, int x, int y, std::string text, const emk::Font & font) {
      cur_text = new emk::Text(x, y, text, font);
      if (text_map.find(name) != text_map.end()) delete text_map[name]; // If we a replacing a text, delete the old one.
      text_map[name] = cur_text;
      shape_map[name] = cur_text;
      return *cur_text;
    }

    // If no explicit font is provided for the text use the cur_font (plus other default values).
    Text & AddText(const std::string & name, int x=0, int y=0, std::string text="") { return AddText(name, x, y, text, *cur_font); }


    template<class T> Button & AddButton(const std::string & name, T * target, void (T::*method_ptr)()) {
      cur_button = new emk::Button(target, method_ptr, name);
      if (button_map.find(name) != button_map.end()) delete button_map[name]; // If we a replacing a button, delete the old one.
      button_map[name] = cur_button;
      shape_map[name] = cur_button;
      return *cur_button;
    }

    ButtonSet & AddButtonSet(const std::string & name, int cols, int rows, int x, int y, int width, int height, int spacing=0) {
      cur_buttonset = new emk::ButtonSet(cols, rows, x, y, width, height, spacing);
      if (buttonset_map.find(name) != buttonset_map.end()) delete buttonset_map[name]; // If we a replacing a buttonset, delete the old one.
      buttonset_map[name] = cur_buttonset;
      return *cur_buttonset;
    }

    Grid & AddGrid(const std::string & name, int x, int y, int width, int height, int cols, int rows, int num_colors=12, int border_width=1) {
      cur_grid = new emk::Grid(x, y, width, height, cols, rows, num_colors, border_width);
      if (grid_map.find(name) != grid_map.end()) delete grid_map[name]; // If we a replacing a grid, delete the old one.
      grid_map[name] = cur_grid;
      shape_map[name] = cur_grid;
      return *cur_grid;
    }

    EventChain & AddEventChain(const std::string & name) {
      cur_eventchain = new emk::EventChain();
      if (eventchain_map.find(name) != eventchain_map.end()) delete eventchain_map[name]; // If we a replacing a eventchain, delete the old one.
      eventchain_map[name] = cur_eventchain;
      return *cur_eventchain;
    }

    Tween & AddTween(const std::string & name, Object & target, double seconds) {
      cur_tween = new emk::Tween(target, seconds);
      if (tween_map.find(name) != tween_map.end()) delete tween_map[name]; // If we a replacing a tween, delete the old one.
      tween_map[name] = cur_tween;
      return *cur_tween;
    }



    emk::Stage & Stage(const std::string & name="") {
      if (name != "") cur_stage = stage_map[name];
      assert(cur_stage != NULL);
      return *cur_stage;
    }
    emk::Layer & Layer(const std::string & name="") {
      if (name != "") cur_layer = layer_map[name];
      assert(cur_layer != NULL);
      return *cur_layer;
    }
    emk::Color & Color(const std::string & name="") {
      if (name != "") cur_color = color_map[name];
      assert(cur_color != NULL);
      return *cur_color;
    }
    emk::Font & Font(const std::string & name="") {
      if (name != "") cur_font = font_map[name];
      assert(cur_font != NULL);
      return *cur_font;
    }
    emk::Point & Point(const std::string & name="") {
      if (name != "") cur_point = point_map[name];
      assert(cur_point != NULL);
      return *cur_point;
    }
    emk::Image & Image(const std::string & name="") {
      if (name != "") cur_image = image_map[name];
      assert(cur_image != NULL);
      return *cur_image;
    }
    emk::Rect & Rect(const std::string & name="") {
      if (name != "") cur_rect = rect_map[name];
      assert(cur_rect != NULL);
      return *cur_rect;
    }
    emk::Text & Text(const std::string & name="") {
      if (name != "") cur_text = text_map[name];
      assert(cur_text != NULL);
      return *cur_text;
    }
    emk::Button & Button(const std::string & name="") {
      if (name != "") cur_button = button_map[name];
      assert(cur_button != NULL);
      return *cur_button;
    }
    emk::ButtonSet & ButtonSet(const std::string & name="") {
      if (name != "") cur_buttonset = buttonset_map[name];
      assert(cur_buttonset != NULL);
      return *cur_buttonset;
    }
    emk::Grid & Grid(const std::string & name="") {
      if (name != "") cur_grid = grid_map[name];
      assert(cur_grid != NULL);
      return *cur_grid;
    }
    emk::EventChain & EventChain(const std::string & name="") {
      if (name != "") cur_eventchain = eventchain_map[name];
      assert(cur_eventchain != NULL);
      return *cur_eventchain;
    }
    emk::Tween & Tween(const std::string & name="") {
      if (name != "") cur_tween = tween_map[name];
      assert(cur_tween != NULL);
      return *cur_tween;
    }

  };

};

#endif
