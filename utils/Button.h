#ifndef EMK_BUTTON_H
#define EMK_BUTTON_H

#include "../Kinetic.h"

namespace emk {
  
  class Button : public emkCustomShape {
  private:
    bool is_active;  // Is this button currently functional?
    bool mouse_down;

    emkCallback * trigger_cb;
    emkCallback * draw_icon_cb;

    // By default, this will be a square button with optionally rounded corners, but the draw function can be replaced.
    bool ul_round;
    bool ur_round;
    bool ll_round;
    bool lr_round;
  public:
    template<class T> Button(T * in_target, void (T::*in_method_ptr)())
      : emkCustomShape(this, &Button::Default_Draw), is_active(true), mouse_down(false)
      , ul_round(true), ur_round(true), ll_round(true), lr_round(true)
    {
      trigger_cb = new emkMethodCallback<T>(in_target, in_method_ptr);
      draw_icon_cb = NULL;
      On("mousedown", this, &Button::Default_OnMouseDown);
      On("mouseup", this, &Button::Default_OnMouseUp);
    }
    virtual ~Button() { ; }

    void SetActive(bool _in=true) { is_active = _in; }
    void SetRoundCorners(bool _ul, bool _ur, bool _ll, bool _lr) {
      ul_round = _ul;      ur_round = _ur;
      ll_round = _ll;      lr_round = _lr;
    }

    virtual void Toggle() { ; }  // If this is a toggle button, switch its state.
    void Trigger() { ; }  // @CAO -- trigger this button's effect!
    void UpdateHover() { ; } // @CAO -- figure out what to do here.

    // A few default behaviors...
    void Default_Draw(emkCanvas & canvas) { ; } // @CAO -- do some reasonable default behavior.

    void Default_OnMouseDown(const emkEventInfo &) {
      if (is_active == false) return;
      mouse_down = true;
      DrawLayer();
    }

    void Default_OnMouseUp(const emkEventInfo &) {
      if (is_active == false) return;
      mouse_down = false;
      Toggle();                         // If this is a toggle button...
      Trigger(); // @CAO Make sure to update board if relevant!
      UpdateHover();
      DrawLayer();
      // UpdateTooltip(this.x, this.y, this.tooltip); // ?????
    }
    
  
  };

  class ToggleButton : public Button {
  private:
    bool is_pressed;

  public:
    template<class T> ToggleButton(T * in_target, void (T::*in_method_ptr)())
      : Button(in_target, in_method_ptr), is_pressed(false) { ; }
    ~ToggleButton() { ; }

    void Toggle() { is_pressed = !is_pressed; }
    
    bool IsPressed() const { return is_pressed; }
  };
  
};

#endif
