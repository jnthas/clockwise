
#ifndef FakeGFX_h
#define FakeGFX_h

#include <Arduino.h>

class FakeGFX {
  private:
    boolean display[64][64];
    void drawScreen();
    
  public:
    FakeGFX();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void refresh();
    
};


#endif


