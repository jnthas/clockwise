
#include "Display.h"

const unsigned short MASK = 0xF81F;


Display::Display(Adafruit_GFX* display) {
  _display = display;
}


void Display::draw(const unsigned short* image_array, int x, int y, int w, int h)
{  
  relativeDraw(image_array, x, y, 0, 0, w, h);
}

void Display::drawMask(const unsigned short* image_array, const byte* mask_array, int x, int y, int w, int h)
{  
  int anchor = 0;
  for (int yy = 0; yy < h; yy++)
  {    
    //Serial.printf("Line %d\n", yy);
    for (int xx = 0; xx < w; xx++)
    {
      if (image_array[anchor] != MASK) {

        if (xx < 64 && mask_array[xx + (yy*64)] != 0) {
          //Serial.printf("%d,", xx + (yy*64));
          
          _display->drawPixel(xx + x , yy + y, COLOR_UTIL.brighter(image_array[anchor], mask_array[xx + (yy*64)]));
        } else {
          
          _display->drawPixel(xx + x , yy + y, image_array[anchor]);
        }
        
      }
      anchor++;
    }
  }

}

void Display::relativeDraw(const unsigned short* image_array, int x, int y, int anchorX, int anchorY, int w, int h)
{  
  int anchor = anchorX;
  for (int yy = anchorY; yy < h; yy++)
  {    
    for (int xx = 0; xx < w; xx++)
    {
      if (image_array[anchor] != MASK) {
        _display->drawPixel(xx + x , yy + y, image_array[anchor]);
      }
      anchor++;
    }
  }
}

void Display::croppedDraw(const unsigned short* image_array, int x, int y, int anchorX, int anchorY, int cropX, int cropY, int w, int h)
{  
  int anchor = anchorX;
  for (int yy = anchorY; yy < cropY; yy++)
  {    
    for (int xx = 0; xx < cropX; xx++)
    {
      if (image_array[anchor] != MASK) {
        this->drawPixel(xx + x , yy + y, image_array[anchor]);
      }

      anchor++;
    }
    anchor = anchor + (w - cropX);
  }
}


void Display::croppedDrawMask(const unsigned short* image_array, const byte* mask_array, int x, int y, int anchorX, int anchorY, int cropX, int cropY, int w, int h)
{  
  int anchor = anchorX;
  for (int yy = anchorY; yy < cropY; yy++)
  {    
    for (int xx = 0; xx < cropX; xx++)
    {
      if (image_array[anchor] != MASK) {

        if (xx+x < 64) { 
                   
          this->drawPixel(xx + x , yy + y, COLOR_UTIL.brighter(image_array[anchor], mask_array[xx+x+((yy+y)*64)]));
        } else {
          this->drawPixel(xx + x , yy + y, image_array[anchor]);
        }
      }

      anchor++;
    }
    anchor = anchor + (w - cropX);
  }

}


void Display::drawPixel(int32_t x, int32_t y, uint16_t color)
{ 
  _display->drawPixel(x, y, color);   

}

void Display::fillRect(int x, int y, int w, int h, uint16_t color) 
{
  _display->fillRect(x, y, w, h, color);
}

void Display::drawRect(int x, int y, int w, int h, uint16_t color) 
{
  _display->drawRect(x, y, w, h, color);
}

void Display::drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
  _display->drawCircle(x0, y0, r, color);
}

void Display::setCursor(int16_t x, int16_t y) 
{
  _display->setCursor(x, y);
}

void Display::print(String str) 
{
  // Serial.print("Display.print -> ");
  // Serial.println(str);
  _display->print(str);
}

void Display::println(String str) 
{
  // Serial.print("Display.println -> ");
  // Serial.println(str);
  _display->println(str);
}

void Display::setTextWrap(boolean wrapX, boolean wrapY)
{
  _display->setTextWrap(wrapX);
}

void Display::setTextColor(uint16_t c) {
  _display->setTextColor(c);
}

void Display::setFont(const GFXfont *f) {
  _display->setFont(f);
}

void Display::setTextSize(uint8_t size) {
  _display->setTextSize(size);
}

int16_t Display::getTextWidth(const char *string) {  

  int16_t  x, y = 0;
  int16_t  x1, y1 = 0;
  uint16_t w, h = 0;

  _display->getTextBounds(string, x, y, &x1, &y1, &w, &h);
  
  return w;
}

void Display::drawHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
  _display->drawFastHLine(x, y, w, color);
}

void Display::drawVLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
  _display->drawFastVLine(x, y, w, color);
}


void Display::refresh() {
  //_display->refresh();
}
