#ifndef VECTOR2_HPP
#define VECTOR2_HPP

class Vector2
{
public:
  double x;
  double y;

  Vector2();
  Vector2(double x, double y);

  void Add(Vector2 other);
  void Scale(double scale);
  void Rotate(double angle);
  double Length(void);
  double SquareLength(void);
  void Normalise(void);
  void CloneFrom(Vector2 other);
  double DotProduct(Vector2 other);
  double AngleBetween(Vector2 other);
};

#endif