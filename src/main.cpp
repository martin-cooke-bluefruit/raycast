#include <Arduino.h>
#include <U8g2lib.h>

#include "Vector2.hpp"
#include "Raycaster.hpp"

#define ANALOG_PIN_1 15

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 23, /* data=*/ 22, /* cs=*/ 19, /* dc=*/ 20, /* reset=*/ 21);

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

unsigned char *displayBuffer;

Vector2 playerPosition;
double playerAngleInRadians;

Raycaster *raycaster;

void HandleInput(void);
void UpdateGame(void);
void Render(void);
void Raycast(uint16_t angle, double *length);
void ApplyDither(unsigned char *data, int error);

void setup() {
  playerPosition.x = 22;
  playerPosition.y = 12;
  playerAngleInRadians = PI / 2.0;

  raycaster = new Raycaster(MAP_WIDTH, MAP_HEIGHT, *worldMap);

  Serial.begin(9600);
  unsigned int rngSeed = analogRead(ANALOG_PIN_1);
  randomSeed(rngSeed);

  displayBuffer = new unsigned char[128 * 64];

  u8g2.begin();
}

void loop() 
{
  HandleInput();
  UpdateGame();
  Render();
}

void HandleInput()
{
  // temp: spin anticlockwise
  playerAngleInRadians += PI / 20.0;
  if (playerAngleInRadians >= 2.0 * PI)
    playerAngleInRadians -= 2.0 * PI;
}

void UpdateGame()
{
  raycaster->SetCameraPosition(playerPosition);
  raycaster->SetCameraDirection(playerAngleInRadians);
  raycaster->RenderToBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, displayBuffer);
  // memset(displayBuffer, 0, 128*64);
  // memset(displayBuffer + 128*16, 200, 128*32);
}

void Render()
{
  u8g2.clearBuffer();
  uint8_t *oledBuffer = u8g2.getBufferPtr();

  for (uint8_t y = 0; y < 64; y++)
  {
    for (uint8_t x = 0; x < 128; x++)
    {
      // ptr offset = y * 128 + x
      unsigned char pixelValue = *(displayBuffer + (y << 7) + x);
      if (pixelValue >= 128)
      {
        // y >> 3     divide by 8 (round down) to find the oled 'page' (rows of 8px high)
        // << 7       multiply by 128 to account for all columns of previous pages
        // + x        add the x-coordinate as a byte offset within the current page
        const uint16_t byteOffset = ((y >> 3) << 7) + x;

        // y & 0x07   the lower 3 bits of the y-coordinate give the vertical pixel position within the current page
        *(oledBuffer + byteOffset) |= 1 << (y & 0x07);
      }

      int ditherError;
      if (pixelValue >= 128)
        ditherError = pixelValue - 255;
      else
        ditherError = pixelValue;

      if (ditherError == 0) 
        continue;
      
      // Floyd-Steinberg dithering
      if (x < 127)
        ApplyDither(displayBuffer + (y << 7)       + x + 1, ditherError * 7 / 16.0);
      if (x > 0 && y < 63)
        ApplyDither(displayBuffer + ((y + 1) << 7) + x - 1, ditherError * 3 / 16.0);
      if (y < 63)
        ApplyDither(displayBuffer + ((y + 1) << 7) + x,     ditherError * 5 / 16.0);
      if (x < 127 && y < 63)
        ApplyDither(displayBuffer + ((y + 1) << 7) + x + 1, ditherError     / 16.0);
    }
  }
  u8g2.sendBuffer();
}

void ApplyDither(unsigned char *data, int error)
{
  int overflowCheck = (int)(*data) + error;
  if (overflowCheck >= 255)
    *data = 255;
  else if (overflowCheck <= 0)
    *data = 0;
  else
    *data += error;
}
