#ifndef INPUT_HPP
#define INPUT_HPP

#include <Arduino.h>
#include <map>

enum Button
{
  Up,
  Down,
  Left,
  Right,
  A,
  B
};

void Input_InitPins(void);
void Input_Clear(void);

bool Input_IsHeld(Button button);
bool Input_WasPressed(Button button);
bool Input_WasReleased(Button button);

#endif