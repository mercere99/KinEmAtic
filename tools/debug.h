#ifndef EMK_ASSERT_H
#define EMK_ASSERT_H

#include <string>

///////////////////// Debug
#ifdef EMK_DEBUG

namespace emk {
  int assert_trip_count = 0;
}

#define assert(EXPR) if ( !(EXPR) && emk::assert_trip_count++ < 3 ) emk::Alert(std::string("Assert Error (In ") + std::string(__FILE__) + std::string(" line ") + std::to_string(__LINE__) + std::string("): ") + std::string(#EXPR))

  ///////////////////// NOT Debug
#else // EMK_DEBUG

#define assert(EXPR) ((void) 0)

#endif // EMK_DEBUG


#endif // EMK_DEBUG_H
