#include "Input.hpp"

#include <Arduino.h>
#include <queue>

#define PIN_UP    17
#define PIN_DOWN  18
#define PIN_LEFT  16
#define PIN_RIGHT 15
#define PIN_A      1
#define PIN_B      0

static std::map<Button, bool> isHeld;
static std::map<Button, bool> wasPressed;
static std::map<Button, bool> wasReleased;

static std::map<Button, uint8_t> buttonToPin;

static std::queue<Button> inputQueue;

static void ToggleUp(void);
static void ToggleDown(void);
static void ToggleLeft(void);
static void ToggleRight(void);
static void ToggleA(void);
static void ToggleB(void);

void Input_InitPins(void)
{
  pinMode(PIN_UP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_UP), ToggleUp, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::Up, (digitalRead(PIN_UP) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Up, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Up, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Up, PIN_UP));

  pinMode(PIN_DOWN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_DOWN), ToggleDown, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::Down, (digitalRead(PIN_DOWN) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Down, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Down, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Down, PIN_DOWN));

  pinMode(PIN_LEFT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_LEFT), ToggleLeft, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::Left, (digitalRead(PIN_LEFT) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Left, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Left, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Left, PIN_LEFT));

  pinMode(PIN_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_RIGHT), ToggleRight, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::Right, (digitalRead(PIN_RIGHT) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Right, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Right, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Right, PIN_RIGHT));

  pinMode(PIN_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_A), ToggleA, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::A, (digitalRead(PIN_A) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::A, false));
  wasReleased.insert(std::pair<Button, bool>(Button::A, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::A, PIN_A));

  pinMode(PIN_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_B), ToggleB, RISING);
  isHeld.insert(std::pair<Button, bool>(Button::B, (digitalRead(PIN_B) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::B, false));
  wasReleased.insert(std::pair<Button, bool>(Button::B, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::B, PIN_B));
}

void Input_Clear(void)
{
  // clear all previous wasPressed & wasReleased states
  for (auto it = wasPressed.begin(); it != wasPressed.end(); ++it)
    it->second = false;

  for (auto it = wasReleased.begin(); it != wasReleased.end(); ++it)
    it->second = false;

  // dequeue any new button presses
  while (inputQueue.size() > 0)
  {
    wasPressed[inputQueue.front()] = true;
    inputQueue.pop();
  }

  for (auto it = buttonToPin.begin(); it != buttonToPin.end(); ++it)
  {
    const Button button = it->first;
    const byte pin = it->second;
    const bool buttonState = digitalRead(pin) == LOW;

    if (isHeld[button] && !buttonState)
      wasReleased[button] = true;

    isHeld[button] = buttonState;
  }
}

bool Input_IsHeld(Button button)
{
  return isHeld[button];
}

bool Input_WasPressed(Button button)
{
  return wasPressed[button];
}

bool Input_WasReleased(Button button)
{
  return wasReleased[button];
}


static void ToggleUp()
{
  inputQueue.push(Button::Up);
}

static void ToggleDown()
{
  inputQueue.push(Button::Down);
}

static void ToggleLeft()
{
  inputQueue.push(Button::Left);
}

static void ToggleRight()
{
  inputQueue.push(Button::Right);
}

static void ToggleA()
{
  inputQueue.push(Button::A);
}

static void ToggleB()
{
  inputQueue.push(Button::B);
}
