#ifndef EMK_BUTTON_H
#define EMK_BUTTON_H

#include <string>

#include "../tools/const.h"
#include "../tools/Colors.h"
#include "../libs/Kinetic.h"

namespace emk {
  
  class Button : public CustomShape {
  private:
    bool is_active;  // Is this button currently functional?
    bool mouse_down;
    bool mouse_over;

    std::string name;
    std::string tooltip;

    Callback * trigger_cb;
    Callback * draw_icon_cb;

    // By default, this will be a square button with optionally rounded corners, but the draw function can be replaced.
    bool ul_round;
    bool ur_round;
    bool ll_round;
    bool lr_round;

    Color color_bg;
    Color color_bg_toggled;
    Color color_bg_mouseover;
    Color color_bg_toggled_mouseover;
    Color color_bg_mousedown;

  public:
    Button(const std::string & _name="")
      : CustomShape(this, &Button::Default_Draw)
      , is_active(true), mouse_down(false), mouse_over(false), name(_name)
      , trigger_cb(NULL), draw_icon_cb(NULL)
      , ul_round(true), ur_round(true), ll_round(true), lr_round(true)
      , color_bg("rgb(255,250,245)"), color_bg_toggled("rgb(255,255,100)"), color_bg_mouseover("rgb(240,240,255)")
      , color_bg_toggled_mouseover("rgb(250,250,200)"), color_bg_mousedown("blue")
    {
      On("mousedown", this, &Button::Default_OnMouseDown);
      On("mouseup", this, &Button::Default_OnMouseUp);
      On("mouseenter", this, &Button::Default_OnMouseEnter);
      On("mouseleave", this, &Button::Default_OnMouseLeave);
    }
    template<class T> Button(T * _target, void (T::*_method_ptr)(), const std::string & _name="")
      : CustomShape(this, &Button::Default_Draw)
      , is_active(true), mouse_down(false), mouse_over(false), name(_name)
      , trigger_cb(NULL), draw_icon_cb(NULL)
      , ul_round(true), ur_round(true), ll_round(true), lr_round(true)
      , color_bg("rgb(255,250,245)"), color_bg_toggled("rgb(255,255,100)"), color_bg_mouseover("rgb(240,240,255)")
      , color_bg_toggled_mouseover("rgb(250,250,200)"), color_bg_mousedown("blue")
    {
      assert(_target && _method_ptr);
      trigger_cb = new emk::MethodCallback<T>(_target, _method_ptr);
      On("mousedown", this, &Button::Default_OnMouseDown);
      On("mouseup", this, &Button::Default_OnMouseUp);
      On("mouseenter", this, &Button::Default_OnMouseEnter);
      On("mouseleave", this, &Button::Default_OnMouseLeave);
    }
    virtual ~Button() { ; }

    void SetActive(bool _in=true) { is_active = _in; }
    void SetToolTip(const std::string & msg) { tooltip = msg; }
    void SetRoundCorners(bool _ul=true, bool _ur=true, bool _lr=true, bool _ll=true) {
      ul_round = _ul;      ur_round = _ur;
      ll_round = _ll;      lr_round = _lr;
    }
    void SetRoundCornerUL(bool round=true) { ul_round = round; }
    void SetRoundCornerUR(bool round=true) { ur_round = round; }
    void SetRoundCornerLR(bool round=true) { lr_round = round; }
    void SetRoundCornerLL(bool round=true) { ll_round = round; }

    void SetFillPatternImage(const Image & _image) {
      // Don't automatically draw the image here, just record it.
      image = &_image;
    }

    void SetBGColor(const Color & _color) { color_bg = _color; }
    void SetBGColorToggled(const Color & _color) { color_bg_toggled = _color; }
    void SetBGColorMouseover(const Color & _color) { color_bg_mouseover = _color; }
    void SetBGColorToggledMouseover(const Color & _color) { color_bg_toggled_mouseover = _color; }
    void SetBGColorMousedown(const Color & _color) { color_bg_mousedown = _color; }

    template<class T> void SetTrigger(T * _target, void (T::*_method_ptr)()) {
      if (trigger_cb != NULL) delete trigger_cb;
      trigger_cb = new emk::MethodCallback<T>(_target, _method_ptr);
    }

    template<class T> void SetupDrawIcon(T * _target, void (T::*_method_ptr)(Canvas &)) {
      if (draw_icon_cb != NULL) delete draw_icon_cb;
      draw_icon_cb = new Callback_Canvas<T>(_target, _method_ptr);
    }

    virtual void Toggle() { ; }  // If this is a toggle button, switch its state.
    virtual bool ToggleOn() const { return false; }
    inline void Trigger() { trigger_cb->DoCallback(); }
    void UpdateHover() { ; } // @CAO -- figure out what to do here.

    // A few default behaviors...
    void Default_Draw(Canvas & canvas)
    {
      const int x = 0; // GetX();
      const int y = 0; // GetY();
      const int width = GetWidth();
      const int height = GetHeight();

      const int x2 = x + width;          // Far X coordinate
      const int y2 = y + height;         // Far Y coordinate
      const int corner_r = 8; //width/5;      // Corner radius of each button

      const int x_in = x + corner_r;     // Inner x for corner arcs
      const int y_in = y + corner_r;     // Inner y for corner arcs
      const int x2_in = x2 - corner_r;   // Inner x2 for corner arcs
      const int y2_in = y2 - corner_r;   // Inner y2 for corner arcs
  
      // Set the button color
      if (mouse_down) canvas.SetFillStyle(color_bg_mousedown);
      else if (mouse_over) {
        if (ToggleOn()) canvas.SetFillStyle(color_bg_toggled_mouseover);
        else canvas.SetFillStyle(color_bg_mouseover);
      }
      else {
        if (ToggleOn()) canvas.SetFillStyle(color_bg_toggled);
        else canvas.SetFillStyle(color_bg);
      }
  
      // Draw the button outline
      canvas.BeginPath();
      canvas.SetLineWidth(ToggleOn() ? 4:2);

      if (ul_round) {
        canvas.MoveTo(x, y_in);
        canvas.Arc(x_in, y_in, corner_r, emk::PI, 3*emk::PI/2);
      } else canvas.MoveTo(x, y);

      if (ur_round) canvas.Arc(x2_in, y_in, corner_r, 3*emk::PI/2, 0);      	  
      else canvas.LineTo(x2, y);

      if (lr_round) canvas.Arc(x2_in, y2_in, corner_r, 0, emk::PI/2);      	  
      else canvas.LineTo(x2, y2);

      if (ll_round) canvas.Arc(x_in, y2_in, corner_r, emk::PI/2, emk::PI);
      else canvas.LineTo(x, y2);

      canvas.ClosePath();
      canvas.Fill();
      canvas.Stroke();

      // Draw the appropriate icon.
      // First, shift the icon to be on a 100-height grid, and shift back afterward.
      canvas.Save();
      if (draw_icon_cb) {
        canvas.Translate(x+5, y+5);
        canvas.Scale( ((double) height-10) / 100.0 );
        draw_icon_cb->DoCallback(); // Write this!
      }
      else if (image) { // otherwise, if there's an image, use it.
        canvas.DrawImage(*image, 0, 0, width, height);
        // canvas.DrawImage(*image, 0, 0, width-10, height-10);
      }
      // @CAO  Else just make it a solid color?
      canvas.Restore();
  
      // Make the button clickable (or grayed out!)
      canvas.BeginPath();
      canvas.SetLineWidth(2);
      if (ul_round) {
        canvas.MoveTo(x, y_in);
        canvas.Arc(x_in, y_in, corner_r, emk::PI, 3*emk::PI/2, false);
      } else canvas.MoveTo(x, y);

      if (ur_round) canvas.Arc(x2_in, y_in, corner_r, 3*emk::PI/2, 0, false);      	  
      else canvas.LineTo(x2, y);

      if (lr_round) canvas.Arc(x2_in, y2_in, corner_r, 0, emk::PI/2, false);      	  
      else canvas.LineTo(x2, y2);
      
      if (ll_round) canvas.Arc(x_in, y2_in, corner_r, emk::PI/2, emk::PI, false);
      else canvas.LineTo(x, y2);

      canvas.ClosePath();
      if (is_active == false) {
        canvas.SetFillStyle("rgba(200,200,200,0.5)");
        canvas.Fill();
      }
      canvas.SetupTarget(*this);
    }

    void Default_OnMouseDown(const emk::EventInfo &) {
      if (is_active == false) return;
      mouse_down = true;
      DrawLayer();
    }

    void Default_OnMouseUp(const emk::EventInfo &) {
      if (is_active == false) return;
      mouse_down = false;
      Toggle();                         // If this is a toggle button...
      Trigger(); // @CAO Make sure to update board if relevant!
      UpdateHover();
      DrawLayer();
      // UpdateTooltip(this.x, this.y, this.tooltip); // ?????
    }
    

    void Default_OnMouseEnter(const emk::EventInfo &) {
      mouse_over = true;
      UpdateHover();
      DrawLayer();
      // UpdateTooltip(this.x, this.y, this.tooltip); // ?????
    }
  
    void Default_OnMouseLeave(const emk::EventInfo &) {
      mouse_over = false;
      mouse_down = false;
      // highlight_id = -1;  // Need to unhighlight and redraw in sudoku... @CAO
      DrawLayer();
      // HideTooltip();
    }
  
  };

  class ToggleButton : public Button {
  private:
    bool is_pressed;

  public:
    template<class T> ToggleButton(T * _target, void (T::*_method_ptr)(), const std::string & _name="")
      : Button(_target, _method_ptr, _name), is_pressed(false) { ; }
    ~ToggleButton() { ; }

    void Toggle() { is_pressed = !is_pressed; }
    bool ToggleOn() const { return is_pressed; }
    
    bool IsPressed() const { return is_pressed; }
  };
  
};

#endif
