#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

#define ANALOG_PIN_1 15

typedef struct {
  double x;
  double y;
} Vector2;

unsigned char levelData[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 1, 0, 0}
};

Vector2 playerPosition;
int16_t playerAngle; // 768 degrees

uint8_t scaledWallHeightAtDisplayColumn[128];

double sinLookup[786];
double cosLookup[786];
double tanLookup[786];

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 23, /* data=*/ 22, /* cs=*/ 19, /* dc=*/ 20, /* reset=*/ 21);

void HandleInput(void);
void UpdateGame(void);
void Render(void);
void Raycast(uint16_t angle, double *length);
double AngleToRadians(uint16_t angle);

void setup() {
  playerPosition.x = 256 * 6.75;
  playerPosition.y = 256 * 3.25;
  playerAngle = 300;

  Serial.begin(9600);
  unsigned int rngSeed = analogRead(ANALOG_PIN_1);
  randomSeed(rngSeed);

  for (uint16_t angle = 0; angle < 768; angle++)
  {
    const double radians = AngleToRadians(angle);
    sinLookup[angle] = sin(radians);
    cosLookup[angle] = cos(radians);
    tanLookup[angle] = tan(radians);
  }

  u8g2.begin();
}

void loop() {

  HandleInput();
  UpdateGame();
  Render();
}

void HandleInput()
{

}

void UpdateGame()
{
  for (uint8_t x = 0; x < 128; x++)
  {
    // 768 - 64 = 704  i.e. ensure angle is +ve before taking mod
    uint16_t angle = (playerAngle + 704 + x) % 768;

    double distanceToWall;
    Raycast(angle, &distanceToWall);

    scaledWallHeightAtDisplayColumn[x] = 8192 / distanceToWall; // 8192 = wall height (64px) * near clip distance (128 units or 0.5 * grid size)
  }
}

void Render()
{
  u8g2.firstPage();

  do {
    for (int x = 0; x < 128; x++)
      u8g2.drawVLine(x, (64 - scaledWallHeightAtDisplayColumn[x]) >> 1, scaledWallHeightAtDisplayColumn[x]);
  } while ( u8g2.nextPage() );
}

void Raycast(uint16_t angle, double *rayLength)
{
  *rayLength = angle + 128;
}

double AngleToRadians(uint16_t angle)
{
  return (angle / 768.0) * (2.0 * PI);
}
