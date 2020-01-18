#include <Arduino.h>

#include "DisplayWrapper.hpp"
#include "Vector2.hpp"
#include "Raycaster.hpp"
#include "Input.hpp"
#include "FPS.hpp"
#include "MelodyPlayer.hpp"
#include <SPI.h>

#define ANALOG_PIN_1 15

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define WALK_SPEED 3.0
#define TURN_SPEED 1.5

int worldMap[MAP_WIDTH][MAP_HEIGHT]=
{
  {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8},
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8},
  {4, 0,13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
  {4, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8},
  {4, 0, 4, 0, 0, 0, 0,13,13,13,13,13,13,13,13,13, 8, 8, 0, 8, 8, 8, 8, 8},
  {4, 0,13, 0, 0, 0, 0,13, 0,13, 0,13, 0,13, 0,13, 8, 0, 0, 0, 8, 8, 8, 8},
  {4, 0, 9, 0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 0,13, 8, 0, 0, 0, 0, 0, 0,14},
  {4, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8},
  {4, 0,14, 0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 0,13, 8, 0, 0, 0, 0, 0, 0,14},
  {4, 0, 0, 0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 0,13, 8, 0, 0, 0, 8, 8, 8, 8},
  {4, 0, 0, 0, 0, 0, 0,13,13,13,13, 0,13,13,13,13, 8, 8, 8, 8, 8, 8, 8, 8},
  {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  {9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
  {9, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
  {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 9, 0, 9, 3, 3, 3, 3, 3, 3, 3,10,10,10, 3},
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 9, 0, 9, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3},
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 3, 0, 0,13, 0, 0, 3, 0, 0, 0, 3},
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 9, 0, 9, 3, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3},
  {4, 0, 9, 0, 9, 0, 0, 0, 0, 4, 9, 0, 0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 3},
  {4, 0, 0,13, 0, 0, 0, 0, 0, 4, 9, 0, 9, 3, 0, 0, 0, 0, 0, 3, 3, 0, 3, 3},
  {4, 0, 9, 0, 9, 0, 0, 0, 0, 4, 9, 0, 9, 3, 0, 0,13, 0, 0, 3, 0, 0, 0, 3},
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 9, 0, 9, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3},
  {4, 4, 4, 4, 4, 4, 4, 4, 4, 4,13,13,13, 3, 3, 3, 3, 3, 3, 3,10,10,10, 3}
};

static DisplayWrapper *display;
static Raycaster *raycaster;
static FPS *fps;

static uint32_t time;

static Vector2 playerPosition;
static Vector2 playerDirection;

void UpdateGame(double deltaTime);
void WalkForward(double distance);
void WalkBackward(double distance);
void StrafeLeft(double distance);
void StrafeRight(double distance);
void TurnLeft(double radians);
void TurnRight(double radians);

void setup()
{
  SPI.setSCK(14);
  display = new DisplayWrapper();

  Serial.begin(9600);
  unsigned int rngSeed = analogRead(ANALOG_PIN_1);
  randomSeed(rngSeed);

  raycaster = new Raycaster(MAP_WIDTH, MAP_HEIGHT, *worldMap);

  fps = new FPS();

  Input_InitPins();

  time = millis();

  playerPosition.x = 17.5;
  playerPosition.y = 22.5;

  playerDirection.x = 1;
  playerDirection.y = 0;
  playerDirection.Normalise();

  int testMelody[] =
  {
    440, 200, 0, 66,
    440, 100, 0, 33,
    440, 100, 0, 33,
    330, 100, 0, 33,
    440, 400
  };

  // // rumble / noise
  // int testMelody[1000];
  // for (int i = 0; i < 1000; i += 2)
  // {
  //   testMelody[i] = random(100, 200);
  //   testMelody[i + 1] = 2;
  // }

  MelodyPlayer_SetPassiveBuzzerPin(22);
  MelodyPlayer_SetMelody(testMelody, sizeof(testMelody) / sizeof(int));
}

void loop()
{
  const uint32_t currentTime = millis();
  const double deltaTime = (currentTime - time) / 1000.0;
  time = currentTime;

  UpdateGame(deltaTime);
  Input_Clear();
  display->Render(true); // dithering parameter
}

void UpdateGame(double deltaTime)
{
  if (Input_IsHeld(Button::Up))
    WalkForward(WALK_SPEED * deltaTime);
  if (Input_IsHeld(Button::Down))
    WalkBackward(WALK_SPEED * deltaTime);
  if (Input_WasPressed(Button::A))
    MelodyPlayer_Play();
  // if (Input_WasPressed(Button::B))
  //   tone(22, 330, 250);
  // if (Input_IsHeld(Button::A))
  //   StrafeLeft(WALK_SPEED * deltaTime);
  // if (Input_IsHeld(Button::B))
  //   StrafeRight(WALK_SPEED * deltaTime);
  if (Input_IsHeld(Button::Left))
    TurnLeft(TURN_SPEED * deltaTime);
  if (Input_IsHeld(Button::Right))
    TurnRight(TURN_SPEED * deltaTime);

  display->Clear();
  raycaster->SetCameraPosition(playerPosition);
  raycaster->SetCameraDirection(playerDirection);
  raycaster->RenderToDisplay(display);
  fps->ShowFPS(display, deltaTime);
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

void StrafeLeft(double distance)
{
  Vector2 delta;
  delta.CloneFrom(playerDirection);
  const double temp = delta.x;
  delta.x = -delta.y;
  delta.y = temp;
  delta.Scale(distance);
  playerPosition.Add(delta);
}

void StrafeRight(double distance)
{
  StrafeLeft(-distance);
}

void TurnLeft(double radians)
{
  playerDirection.Rotate(radians);
}

void TurnRight(double radians)
{
  playerDirection.Rotate(-radians);
}
