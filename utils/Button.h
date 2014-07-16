#ifndef EMK_BUTTON_H
#define EMK_BUTTON_H

#include <string>

#include "../tools/const.h"
#include "../Kinetic.h"

namespace emk {
  
  class Button : public emkCustomShape {
  private:
    bool is_active;  // Is this button currently functional?
    bool mouse_down;
    bool mouse_over;

    Callback * trigger_cb;
    Callback * draw_icon_cb;

    std::string name;
    std::string tooltip;

    // By default, this will be a square button with optionally rounded corners, but the draw function can be replaced.
    bool ul_round;
    bool ur_round;
    bool ll_round;
    bool lr_round;
  public:
    template<class T> Button(T * _target, void (T::*_method_ptr)(), const std::string & _name="")
      : emkCustomShape(this, &Button::Default_Draw)
      , is_active(true), mouse_down(false), mouse_over(false), name(_name)
      , ul_round(true), ur_round(true), ll_round(true), lr_round(true)
    {
      trigger_cb = new emk::MethodCallback<T>(_target, _method_ptr);
      draw_icon_cb = NULL;
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

    void SetFillPatternImage(const emkImage & _image) {
      // Don't automatically draw the image here, just record it.
      image = &_image;
    }

    template<class T> void SetupDrawIcon(T * _target, void (T::*_method_ptr)(emkCanvas &)) {
      draw_icon_cb = new emkCallback_Canvas<T>(_target, _method_ptr);
    }

    virtual void Toggle() { ; }  // If this is a toggle button, switch its state.
    virtual bool ToggleOn() const { return false; }
    inline void Trigger() { trigger_cb->DoCallback(); }
    void UpdateHover() { ; } // @CAO -- figure out what to do here.

    // A few default behaviors...
    void Default_Draw(emkCanvas & canvas)
    {
      const int x = 0; // GetX();
      const int y = 0; // GetY();
      const int width = GetWidth();
      const int height = GetHeight();

      const int x2 = x + width;          // Far X coordinate
      const int y2 = y + height;         // Far Y coordinate
      const int corner_r = width/5;      // Corner radius of each button

      const int x_in = x + corner_r;     // Inner x for corner arcs
      const int y_in = y + corner_r;     // Inner y for corner arcs
      const int x2_in = x2 - corner_r;   // Inner x2 for corner arcs
      const int y2_in = y2 - corner_r;   // Inner y2 for corner arcs
  
      // Set the button color
      if (mouse_down) canvas.SetFillStyle("blue");
      else if (mouse_over) {
        if (ToggleOn()) canvas.SetFillStyle("rgb(250,250,200)");
        else canvas.SetFillStyle("rgb(240,240,255)");
      }
      else {
        if (ToggleOn()) canvas.SetFillStyle("rgb(255,255,100)");
        else canvas.SetFillStyle("rgb(255,250,245)");
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
      // First, shift the icon to be on a 100x100 grid, and shift back afterward.
      canvas.Save();
      if (draw_icon_cb) {
        canvas.Translate(x+5, y+5);
        canvas.Scale(((double)(width-10))/100.0, ((double)(height-10))/100.0);
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
