#ifndef EMK_FONT_H
#define EMK_FONT_H

#include <string>

#include "Colors.h"

namespace emk {
  class Font {
  private:
    std::string family;
    int size;
    Color color;

    bool is_bold;
    bool is_italic;

  public:
    Font(const Font & _in) : family(_in.family), size(_in.size), color(_in.color), is_bold(_in.is_bold), is_italic(_in.is_italic) { ; }
    Font(const std::string & _family="Helvetica", int _size=30, const Color & _color="black", bool _bold=false, bool _italic=false)
      : family(_family), size(_size), color(_color), is_bold(_bold), is_italic(_italic)  { ; }
    Font(int _size, const Color & _color="black", bool _bold=false, bool _italic=false)
      : family("Helvetica"), size(_size), color(_color), is_bold(_bold), is_italic(_italic)  { ; }
    ~Font() { ; }

    const Font & operator=(const Font & _in) { 
      family = _in.family;     size = _in.size;     color = _in.color;     is_bold = _in.is_bold;     is_italic = _in.is_italic;
      return *this;
    }

    const std::string & GetFamily() const { return family; }
    int GetSize() const { return size; }
    const emk::Color & GetColor() const { return color; }
    bool IsBold() const { return is_bold; }
    bool IsItalic() const { return is_italic; }

    Font & SetFamily(const std::string & _family) { family = _family; return *this; }
    Font & SetSize(int _size) { size = _size; return *this; }
    Font & SetColor(const emk::Color & _color) { color = _color; return *this; }
    Font & SetBold(bool _in=true) { is_bold = _in; return *this; }
    Font & SetItalic(bool _in=true) { is_italic = _in; return *this; }

  };
};

#endif
