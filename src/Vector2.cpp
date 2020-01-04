#include "Vector2.hpp"

#include <math.h>

Vector2::Vector2(): x(0), y(0) { }

Vector2::Vector2(double x, double y)
{
  this->x = x;
  this->y = y;
}

void Vector2::Add(const Vector2 other)
{
  x += other.x;
  y += other.y;
}

void Vector2::Scale(double scale)
{
  x *= scale;
  y *= scale;
}

void Vector2::Rotate(const double radians)
{
  const double oldx = x;
  const double oldy = y;

  x = cos(radians) * oldx - sin(radians) * oldy;
  y = sin(radians) * oldx + cos(radians) * oldy;
}
