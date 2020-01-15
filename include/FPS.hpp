#ifndef FPS_HPP
#define FPS_HPP

#include "DisplayWrapper.hpp"

class FPS
{
public:
  void ShowFPS(DisplayWrapper *display, double deltaTime);
};

#endif