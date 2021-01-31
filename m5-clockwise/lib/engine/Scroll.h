#ifndef Scroll_h
#define Scroll_h

#include <Arduino.h>
#include "Display.h"

class Scroll {
  private:
    int _anchorX = 0;
    int _anchorY = 0;

    int _startX; 
    int _startY; 
    int _width; 
    int _height;

  
  public:
    Scroll(int startX, int startY, int w, int h);
    void horizontalScroll(const unsigned short frame[], Display* display, int step);
    void render();

};

#endif 
