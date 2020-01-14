#include "Input.hpp"

#include <Arduino.h>

#define PIN_UP    17
#define PIN_DOWN  18
#define PIN_LEFT  16
#define PIN_RIGHT 15
#define PIN_A      1
#define PIN_B      0

#define DEBOUNCE_DELAY_MS 5

static std::map<Button, bool> isHeld;
static std::map<Button, bool> wasPressed;
static std::map<Button, bool> wasReleased;

static std::map<Button, uint8_t> buttonToPin;

static uint32_t debounceDelayUp;
static uint32_t debounceDelayDown;
static uint32_t debounceDelayLeft;
static uint32_t debounceDelayRight;
static uint32_t debounceDelayA;
static uint32_t debounceDelayB;

static void SetButtonState(Button button, bool state);
static void ToggleUp(void);
static void ToggleDown(void);
static void ToggleLeft(void);
static void ToggleRight(void);
static void ToggleA(void);
static void ToggleB(void);

void Input_InitPins(void)
{
  pinMode(PIN_UP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_UP), ToggleUp, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::Up, (digitalRead(PIN_UP) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Up, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Up, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Up, PIN_UP));
  debounceDelayUp = 0;

  pinMode(PIN_DOWN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_DOWN), ToggleDown, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::Down, (digitalRead(PIN_DOWN) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Down, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Down, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Down, PIN_DOWN));
  debounceDelayDown = 0;

  pinMode(PIN_LEFT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_LEFT), ToggleLeft, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::Left, (digitalRead(PIN_LEFT) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Left, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Left, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Left, PIN_LEFT));
  debounceDelayLeft = 0;

  pinMode(PIN_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_RIGHT), ToggleRight, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::Right, (digitalRead(PIN_RIGHT) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::Right, false));
  wasReleased.insert(std::pair<Button, bool>(Button::Right, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::Right, PIN_RIGHT));
  debounceDelayRight = 0;

  pinMode(PIN_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_A), ToggleA, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::A, (digitalRead(PIN_A) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::A, false));
  wasReleased.insert(std::pair<Button, bool>(Button::A, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::A, PIN_A));
  debounceDelayA = 0;

  pinMode(PIN_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_B), ToggleB, CHANGE);
  isHeld.insert(std::pair<Button, bool>(Button::B, (digitalRead(PIN_B) == LOW)));
  wasPressed.insert(std::pair<Button, bool>(Button::B, false));
  wasReleased.insert(std::pair<Button, bool>(Button::B, false));
  buttonToPin.insert(std::pair<Button, uint8_t>(Button::B, PIN_B));
  debounceDelayB = 0;
}

void Input_Clear(void)
{
  // to do: ensure input is properly queued & consumed - the current approach leads to button
  // clicks just before a call to the input handler being ignored!!

  // clear any was pressed / released flags
for (auto it = wasPressed.begin(); it != wasPressed.end(); ++it)
    it->second = false;

  for (auto it = wasReleased.begin(); it != wasReleased.end(); ++it)
    it->second = false;
}

bool Input_IsHeld(Button button)
{
  return (digitalRead(buttonToPin[button]) == LOW);
  //return isHeld[button];
}

bool Input_WasPressed(Button button)
{
  return wasPressed[button];
}

bool Input_WasReleased(Button button)
{
  return wasReleased[button];
}



static void SetButtonState(Button button, bool state)
{
  isHeld[button] = state;
  if (isHeld[button])
    wasPressed[button] = true;
  else
    wasReleased[button] = true;
}

static void ToggleUp()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayUp)
  {
    debounceDelayUp = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::Up, (digitalRead(PIN_UP) == LOW));
  }
}

static void ToggleDown()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayDown)
  {
    debounceDelayDown = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::Down, (digitalRead(PIN_DOWN) == LOW));
  }
}

static void ToggleLeft()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayLeft)
  {
    debounceDelayLeft = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::Left, (digitalRead(PIN_LEFT) == LOW));
  }
}

static void ToggleRight()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayRight)
  {
    debounceDelayRight = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::Right, (digitalRead(PIN_RIGHT) == LOW));
  }
}

static void ToggleA()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayA)
  {
    debounceDelayA = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::A, (digitalRead(PIN_A) == LOW));
  }
}

static void ToggleB()
{
  uint32_t currentTime = millis();
  if (currentTime >= debounceDelayB)
  {
    debounceDelayB = currentTime + DEBOUNCE_DELAY_MS;
    SetButtonState(Button::B, (digitalRead(PIN_B) == LOW));
  }
}
