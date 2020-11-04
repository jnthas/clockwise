#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include "Adafruit_GFX.h"

class Display {
  private:
    Adafruit_GFX* _display;

  public:
    Display(Adafruit_GFX* display);
    void draw(const unsigned short frame[], int x, int y, int w, int h);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void setTextColor(uint16_t c);
    void setCursor(int16_t x, int16_t y);
    void print(String str);

    void refresh();
    
};




#endif