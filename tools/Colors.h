#ifndef EMK_COLORS_H
#define EMK_COLORS_H

#include <iomanip>
#include <string>
#include <sstream>

namespace emk {
  class Color {
  private:
    std::string colorname;

  public:
    Color() { ; }
    Color(const std::string & _name) : colorname(_name) { ; }
    Color(int r, int g, int b) { // @CAO This is technically shorter than "rgb(##,##,##)", but more processing.
      std::stringstream stream;
      stream << '#' << std::setw(2) << std::setfill('0') << std::hex << r
             << std::setw(2) << std::setfill('0') << std::hex << g
             << std::setw(2) << std::setfill('0') << std::hex << b;
      colorname = stream.str();
      EMK_Alert(colorname.c_str());
    }
    Color(int r, int g, int b, double a) {
      std::stringstream stream;
      stream << "rgba(" << r << ',' << g << ',' << b << ',' << a << ')';
      colorname = stream.str();
    }

    const std::string & AsString() const { return colorname; }
  };

}

#endif

