#include <Arduino.h>

#include "DisplayWrapper.hpp"
#include "Vector2.hpp"
#include "Raycaster.hpp"
#include "Input.hpp"

#define ANALOG_PIN_1 15

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define WALK_SPEED 3.0
#define TURN_SPEED 1.5

int worldMap[MAP_WIDTH][MAP_HEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

DisplayWrapper *display;

Vector2 playerPosition;
Vector2 playerDirection;

Raycaster *raycaster;

static uint32_t time;

void UpdateGame(void);

void WalkForward(double distance);
void WalkBackward(double distance);
void TurnLeft(double radians);
void TurnRight(double radians);

void setup()
{
  display = new DisplayWrapper();

  Serial.begin(9600);
  unsigned int rngSeed = analogRead(ANALOG_PIN_1);
  randomSeed(rngSeed);

  raycaster = new Raycaster(MAP_WIDTH, MAP_HEIGHT, *worldMap);

  Input_InitPins();

  time = millis();

  playerPosition.x = 12;
  playerPosition.y = 12;

  playerDirection.x = 0;
  playerDirection.y = 1;
}

void loop() 
{
  UpdateGame();
  Input_Clear();
  display->Render(true); // dithering parameter
}

void UpdateGame()
{
  uint32_t currentTime = millis();
  double delatTime = (currentTime - time) / 1000.0;
  time = currentTime;

  if (Input_IsHeld(Button::Up))
    WalkForward(WALK_SPEED * delatTime);
  if (Input_IsHeld(Button::Down))
    WalkBackward(WALK_SPEED * delatTime);
  if (Input_IsHeld(Button::Left))
    TurnLeft(TURN_SPEED * delatTime);
  if (Input_IsHeld(Button::Right))
    TurnRight(TURN_SPEED * delatTime);

  display->Clear();

  raycaster->SetCameraPosition(playerPosition);
  raycaster->SetCameraDirection(playerDirection);
  raycaster->RenderToBuffer(display->GetWidth(), display->GetHeight(), display->GetBuffer());
}

void WalkForward(double distance)
{
  Vector2 delta;
  delta.CloneFrom(playerDirection);
  delta.Scale(distance);
  playerPosition.Add(delta);
}

void WalkBackward(double distance)
{
  WalkForward(-distance);
}

void TurnLeft(double radians)
{
  playerDirection.Rotate(radians);
}

void TurnRight(double radians)
{
  playerDirection.Rotate(-radians);
}
