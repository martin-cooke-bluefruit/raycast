#ifndef DISPLAY_WRAPPER_HPP
#define DISPLAY_WRAPPER_HPP

#include <U8g2lib.h>

class DisplayWrapper
{
public:
  DisplayWrapper(void);

  void Clear(void);
  unsigned char* GetBuffer(void);
  void Render(bool dither);
  unsigned int GetWidth(void);
  unsigned int GetHeight(void);

private:
  //U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2;
  unsigned char *displayBuffer;
  int bufferSize;

  void ApplyDither(unsigned char *buffer, uint16_t offset, int error);
};

#endif