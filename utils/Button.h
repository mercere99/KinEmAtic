#ifndef EMK_BUTTON_H
#define EMK_BUTTON_H

namespace emk {
  
  class Button {
  private:
    bool is_active;
  public:
    Button() : is_active(true) { ; }
    ~Button() { ; }

    void SetActive(bool _in=true) { is_active = _in; }
  };

  class ToggleButton : public Button {
  private:
    bool is_pressed;

  public:
    ToggleButton() : is_pressed(false) { ; }
    ~ToggleButton() { ; }

    void Toggle() { is_pressed = !is_pressed; }
  };
  
};

#endif
