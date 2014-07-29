#ifndef EMK_COORDS_H
#define EMK_COORDS_H

#include <cmath>

namespace emk {

  class Coords {
  private:
    int x;
    int y;

  public:
    Coords(int _x, int _y) : x(_x), y(_y) { ; }
    ~Coords() { ; }

    inline int GetX() const { return x; }
    inline int GetY() const { return y; }

    Coords & Set(int _x, int _y) { x=_x; y=_y; return *this; }

    double Magnitude() const { return sqrt( (double) (x * x + y * y) ); }

    Coords operator+(const Coords & _in) const { return Coords(GetX() + _in.GetX() , GetY() + _in.GetY()); }
    Coords operator-(const Coords & _in) const { return Coords(GetX() - _in.GetX() , GetY() - _in.GetY()); }
    Coords & operator+=(const Coords & _in) { x += _in.GetX(); y += _in.GetY(); return *this; }
    Coords & operator-=(const Coords & _in) { x -= _in.GetX(); y -= _in.GetY(); return *this; }

    Coords & Rot90() { return Set(y, -x); }
    Coords & Rot180() { return Set(-x, -y); }
    Coords & Rot270() { return Set(-y, x); }
  };

};

#endif
