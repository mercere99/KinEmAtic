#ifndef EMK_FUNCTIONS_H
#define EMK_FUNCTIONS_H

#include "Callbacks.h"

extern "C" {
  extern void EMK_ScheduleCallback(int callback_ptr, int delay);
}

namespace emk {
  template<class T> void ScheduleMethod(T * target, void (T::*method_ptr)(), int delay) {
    MethodCallback<T> * new_callback = new MethodCallback<T>(target, method_ptr);
    new_callback->SetDisposible(); // Get rid of this object after one use.
    EMK_ScheduleCallback((int) new_callback, delay);
  }

  // These may already be in HTML5 for Emscripten
  void SetCursor(const char * type) {
    EM_ASM_ARGS({
        var type = Pointer_stringify($0);
        document.body.style.cursor = type;
    }, type);
  }

};

#endif
