
#include "Display.h"

const unsigned short MASK = 0xF81F;


Display::Display(Adafruit_GFX* display) {
  _display = display;
}


void Display::draw(const unsigned short frame[], int x, int y, int w, int h)
{  
  int counter = 0;
  for (int yy = 0; yy < h; yy++)
  {
    for (int xx = 0; xx < w; xx++)
    {
      if (frame[counter] != MASK) {
        _display->drawPixel(xx + x , yy + y, frame[counter]);
      }
      counter++;

    }
  }
}

void Display::fillRect(int x, int y, int w, int h, uint16_t color) 
{
  _display->fillRect(x, y, w, h, color);
}

void Display::setCursor(int16_t x, int16_t y) 
{
  _display->setCursor(x, y);
}

void Display::print(String str) 
{
  _display->print(str);
}

void Display::setTextColor(uint16_t c) {
  _display->setTextColor(c);
}

void Display::setFont(const GFXfont *f) {
  _display->setFont(f);
}

void Display::refresh() {
  //_display->refresh();
}