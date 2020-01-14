#include "DisplayWrapper.hpp"
#include <U8g2lib.h>
#include <string.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// 128 * 8
#define OLED_BUFFER_SIZE 1024

//static U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
static U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

DisplayWrapper::DisplayWrapper(void)
{
  u8g2.begin();

  bufferSize = DISPLAY_WIDTH * DISPLAY_HEIGHT;
  displayBuffer = new unsigned char[bufferSize];
  Clear();
}

unsigned char* DisplayWrapper::GetBuffer(void)
{
  return displayBuffer;
}

void DisplayWrapper::Clear(void)
{
  memset(displayBuffer, 0, bufferSize);
}

unsigned int DisplayWrapper::GetWidth(void)
{
  return DISPLAY_WIDTH;
}

unsigned int DisplayWrapper::GetHeight(void)
{
  return DISPLAY_HEIGHT;
}

void DisplayWrapper::Render(bool dither)
{
  u8g2.clearBuffer();

  uint8_t *oledBuffer = u8g2.getBufferPtr();

  for (uint8_t y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (uint8_t x = 0; x < DISPLAY_WIDTH; x++)
    {
      // ptr offset = y * DISPLAY_WIDTH + x
      unsigned char pixelValue = *(displayBuffer + (y << 7) + x);
      if (pixelValue >= 128) // brightness 50 - 100%, illuminate pixel
      {
        // y >> 3     divide by 8 (round down) to find the oled 'page' (rows of 8px high)
        // << 7       multiply by 128 to account for all columns of previous pages
        // + x        add the x-coordinate as a byte offset within the current page
        const uint16_t byteOffset = ((y >> 3) << 7) + x;
        if (byteOffset >= OLED_BUFFER_SIZE)
          continue;

        // y & 0x07   the lower 3 bits of the y-coordinate give the vertical pixel position within the current page
        *(oledBuffer + byteOffset) |= 1 << (y & 0x07);
      }

      if (dither)
      {
        bool negate = false;

        unsigned char ditherError;
        if (pixelValue >= 128)
        {
          ditherError = 255 - pixelValue;
          negate = true;
        }
        else
          ditherError = pixelValue;

        if (ditherError == 0) 
          continue;
        
        // Floyd-Steinberg dithering
        if (x + 1 < DISPLAY_WIDTH)
          ApplyDither(displayBuffer,   (y << 7) + x + 1,       ((ditherError << 3) - ditherError) >> 4, negate); // ((d * 8) - d) / 16  =  7d / 16
        if (y + 1 < DISPLAY_HEIGHT)
        {
          if (x - 1 >= 0)
            ApplyDither(displayBuffer, ((y + 1) << 7) + x - 1, ((ditherError << 2) - ditherError) >> 4, negate); // ((d * 4) - d) / 16  =  3d / 16

            ApplyDither(displayBuffer, ((y + 1) << 7) + x,     ((ditherError << 2) + ditherError) >> 4, negate); // ((d * 4) + d) / 16  =  5d / 16

          if (x + 1 < DISPLAY_WIDTH)
            ApplyDither(displayBuffer, ((y + 1) << 7) + x + 1, ditherError >> 4, negate);                        //                     =   d / 16
        }
      }
    }
  }
  u8g2.sendBuffer();
}

void DisplayWrapper::ApplyDither(unsigned char *buffer, const uint16_t offset, const unsigned char error, const bool negate)
{
  if (offset >= bufferSize)
    return;
  
  unsigned char oldPixel = *(buffer + offset);
  unsigned char newPixel;
  if (negate)
  {
    newPixel = oldPixel - error;
    if (newPixel > oldPixel)
      newPixel = 0;
  }
  else
  {
    newPixel = oldPixel + error;
    if (newPixel < oldPixel)
      newPixel = 255;
  }

  *(buffer + offset) = newPixel;
  return;
}
