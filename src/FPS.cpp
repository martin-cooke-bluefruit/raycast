#include "FPS.hpp"

void FPS::ShowFPS(DisplayWrapper *display, double deltaTime)
{
  const int fps = 1.0 / deltaTime;
  unsigned char *displayBuffer = display->GetBuffer();

  for (int x = 0; x < 30; x++)
  {
    if (x % 5 == 4)
      *(displayBuffer + x) = 255;
    else
      *(displayBuffer + x) = 0;

    if (x < fps)
      *(displayBuffer + 128 + x) = 255;
    else
      *(displayBuffer + 128 + x) = 0;
  }
}