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

void Vector2::Scale(const double scale)
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

double Vector2::Length(void)
{
  return sqrt((x * x) + (y * y));
}

double Vector2::SquareLength(void)
{
  return (x * x) + (y * y);
}

void Vector2::Normalise(void)
{
  const double length = Length();
  x /= length;
  y /= length;
}

void Vector2::CloneFrom(Vector2 other)
{
  x = other.x;
  y = other.y;
}

double Vector2::DotProduct(Vector2 other)
{
  return (x * other.x) + (y * other.y);
}

double Vector2::AngleBetween(Vector2 other)
{
  return acos(DotProduct(other) / (Length() * other.Length()));
}
