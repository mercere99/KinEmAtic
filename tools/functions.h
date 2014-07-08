#ifndef EMK_FUNCTIONS_H
#define EMK_FUNCTIONS_H

extern "C" {
  extern void EMK_ScheduleCallback(int callback_ptr, int delay);
}

namespace emk {
  template<class T> void ScheduleMethod(T * target, void (T::*method_ptr)(), int delay) {
    emkMethodCallback<T> * new_callback = new emkMethodCallback<T>(target, method_ptr);
    new_callback->SetDisposible(); // Get rid of this object after one use.
    EMK_ScheduleCallback((int) new_callback, delay);
  }
};

#endif
