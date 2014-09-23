#ifndef EMK_TEXT_H
#define EMK_TEXT_H

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  A set of classes to deal with formatted strings.
//
//  SFText = Single-Format text.  A string where everything is a single specific format.
//  TextStream = A text handler that can have multiple formats throughout.
//

#include <string>
#include <sstream>

#include "Font.h"

namespace emk {

  class SFText {
  private:
    std::string text;
    Font font;

  public:
    SFText(std::string _text="", Font _font=Font())
      : text(_text), font(_font) { ; }
    ~SFText() { ; }

    const std::string & GetText() const { return text; }
    const Font & GetFont() const { return font; }

    const std::string & GetFontFamily() const { return font.GetFamily(); }
    int GetFontSize() const { return font.GetSize(); }
    const emk::Color & GetColor() const { return font.GetColor(); }
    bool IsBold() const { return font.IsBold(); }
    bool IsItalic() const { return font.IsItalic(); }

    SFText & SetText(const std::string & _text) { text = _text; return *this; }
    SFText & SetFont(const Font & _font) { font = _font; return *this; }

    SFText & SetFontFamily(const std::string & _family) { font.SetFamily(_family); return *this; }
    SFText & SetFontSize(int _size) { font.SetSize(_size); return *this; }
    SFText & SetColor(const emk::Color & _color) { font.SetColor(_color); return *this; }
    SFText & SetBold(bool _in=true) { font.SetBold(_in); return *this; }
    SFText & SetItalic(bool _in=true) { font.SetItalic(_in); return *this; }

    SFText & Append(const std::string & in_str) { text += in_str; return *this; }

    std::string AsHTMLString() {
      std::stringstream ss;
      std::string format_text(text);

      // Substitute a <br> tag for each newline in the string.
      size_t start_pos;
      while ((start_pos = format_text.find('\n')) != std::string::npos) {
        format_text.replace(start_pos, 1, "<br>");
      }
      
      // Make sure to format the string correctly for the given font.
      ss << font.GetHTMLStart() << format_text << font.GetHTMLEnd();
      return ss.str();
    }
  };


  class TextStream {
  private:
    Font cur_font;
    std::vector<SFText *> text_set;
    bool font_change;
  public:
    TextStream(std::string text="", Font font=Font()) : cur_font(font), font_change(true)
    {
      if (text != "") {
        SFText * new_text = new SFText(text, cur_font);
        text_set.push_back(new_text);
        font_change = false;
      }
    }
    ~TextStream() { ; }

    std::string AsString() {
      std::stringstream ss;
      for (int i = 0; i < (int) text_set.size(); i++) {
        ss << text_set[i]->GetText();
      }
      return ss.str();
    }

    std::string AsHTMLString() {
      std::stringstream ss;
      for (int i = 0; i < (int) text_set.size(); i++) {
        ss << text_set[i]->AsHTMLString();
      }
      return ss.str();
    }
    
    TextStream & operator<<(const std::string & in_str) { 
      if (font_change == false) {
        text_set.back()->Append(in_str);
      }
      else {
        SFText * new_text = new SFText(in_str, cur_font);
        text_set.push_back(new_text);
        font_change = false;
      }

      return *this;
    }

    TextStream & operator<<(const char * in_str) { return operator<<(std::string(in_str)); }

    TextStream & operator<<(char in_char) { 
      if (font_change == false) {
        text_set.back()->Append(std::to_string(in_char));
      }
      else {
        SFText * new_text = new SFText(std::to_string(in_char), cur_font);
        text_set.push_back(new_text);
        font_change = false;
      }

      return *this;
    }

    TextStream & operator<<(int in_int) { 
      if (font_change == false) {
        text_set.back()->Append(std::to_string(in_int));
      }
      else {
        SFText * new_text = new SFText(std::to_string(in_int), cur_font);
        text_set.push_back(new_text);
        font_change = false;
      }

      return *this;
    }

    TextStream & operator<<(float in_float) { 
      if (font_change == false) {
        text_set.back()->Append(std::to_string(in_float));
      }
      else {
        SFText * new_text = new SFText(std::to_string(in_float), cur_font);
        text_set.push_back(new_text);
        font_change = false;
      }

      return *this;
    }

    TextStream & operator<<(const Font & in_font) { 
      if (in_font != cur_font) {
        cur_font = in_font;
        font_change = true;
      }

      return *this;
    }

    TextStream & operator<<(const Color & in_color) { 
      if (cur_font.GetColor() != in_color) {
        cur_font.SetColor(in_color);
        font_change = true;
      }

      return *this;
    }

    TextStream & operator<<( std::ostream&(*f)(std::ostream&) )
    {
      if (f == std::endl) {

        if (font_change == false) {
          text_set.back()->Append(std::to_string('\n'));
        }
        else {
          SFText * new_text = new SFText(std::to_string('\n'), cur_font);
          text_set.push_back(new_text);
          font_change = false;
        }

      }
      
      return *this;
    }

  };

};


#endif
