#ifndef EMK_POINT_H
#define EMK_POINT_H

#include <cmath>

namespace emk {

  class Point {
  private:
    double x;
    double y;

  public:
    Point(const Point & _in) : x(_in.x), y(_in.y) { ; }
    Point(double _x, double _y) : x(_x), y(_y) { ; }
    ~Point() { ; }

    const Point & operator=(const Point & _in) { x = _in.x; y = _in.y; return *this; }

    inline double GetX() const { return x; }
    inline double GetY() const { return y; }

    Point & Set(double _x, double _y) { x=_x; y=_y; return *this; }
    Point & Translate(double shift_x, double shift_y) { x += shift_x; y += shift_y; return *this; }
    Point & TransX(double shift) { x += shift; return *this; }
    Point & TransY(double shift) { y += shift; return *this; }

    double Magnitude() const { return sqrt( (double) (x * x + y * y) ); }

    Point operator+(const Point & _in) const { return Point(GetX() + _in.GetX() , GetY() + _in.GetY()); }
    Point operator-(const Point & _in) const { return Point(GetX() - _in.GetX() , GetY() - _in.GetY()); }
    Point & operator+=(const Point & _in) { x += _in.GetX(); y += _in.GetY(); return *this; }
    Point & operator-=(const Point & _in) { x -= _in.GetX(); y -= _in.GetY(); return *this; }

    Point & Rot90() { return Set(y, -x); }
    Point & Rot180() { return Set(-x, -y); }
    Point & Rot270() { return Set(-y, x); }
  };

};

#endif
