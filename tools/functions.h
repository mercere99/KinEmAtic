#ifndef EMK_FUNCTIONS_H
#define EMK_FUNCTIONS_H

#include "Callbacks.h"

namespace emk {

  inline void Alert(const std::string & msg) { EM_ASM_ARGS({ msg = Pointer_stringify($0); alert(msg); }, msg.c_str()); }
  void Alert(int val) { Alert(std::to_string(val)); }
  void Alert(double val) { Alert(std::to_string(val)); }

#define AlertVar(VAR) emk::Alert(std::string(#VAR) + std::string("=") + std::to_string(VAR))

  template<class T> void ScheduleMethod(T * target, void (T::*method_ptr)(), int delay) {
    MethodCallback<T> * new_callback = new MethodCallback<T>(target, method_ptr);
    new_callback->SetDisposible(); // Get rid of this object after one use.
    EM_ASM_ARGS({ window.setTimeout(function() { emkJSDoCallback($0); }, $1); }, (int) new_callback, delay);
  }

  int GetWindowInnerWidth() { return EM_ASM_INT_V({ return window.innerWidth; }); }
  int GetWindowInnerHeight() { return EM_ASM_INT_V({ return window.innerHeight; }); }

  // These may already be in HTML5 for Emscripten
  void SetCursor(const char * type) {
    EM_ASM_ARGS({
        var type = Pointer_stringify($0);
        document.body.style.cursor = type;
    }, type);
  }

};

#endif
