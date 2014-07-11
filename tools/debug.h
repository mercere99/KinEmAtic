#ifndef EMK_DEBUG_H
#define EMK_DEBUG_H

#include <string>

#define EMK_DEBUG

  ///////////////////// Debug
#ifdef EMK_DEBUG

namespace emk {
  assert_trip_count = 0;
}

#define assert(EXPR) if (!(EXPR) && assert_trip_count++ < 3) emkAlert(std::string("Assert Error: ") + std::string(#EXPR)))

  ///////////////////// NOT Debug
#else // EMK_DEBUG

#define assert(EXPR) ((void) 0)

#endif // EMK_DEBUG


#endif // EMK_DEBUG_H
