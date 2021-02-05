#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include "Adafruit_GFX.h"
#include "ColorUtil.h"

class Display {
  private:
    Adafruit_GFX* _display;

  public:
    Display(Adafruit_GFX* display);
    void draw(const unsigned short* image_array, int x, int y, int w, int h);
    void drawMask(const unsigned short* image_array, const byte* mask_array, int x, int y, int w, int h);
    
    void relativeDraw(const unsigned short* image_array, int x, int y, int anchorX, int anchorY, int w, int h);
    void croppedDraw(const unsigned short* image_array, int x, int y, int anchorX, int anchorY, int cropX, int cropY, int w, int h);
    void croppedDrawMask(const unsigned short* image_array, const byte* mask_array, int x, int y, int anchorX, int anchorY, int cropX, int cropY, int w, int h);
    
    void drawPixel(int32_t x, int32_t y, uint16_t color);
       
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);

    void setTextColor(uint16_t c);
    void setCursor(int16_t x, int16_t y);
    void print(String str);
    void println(String str);
    void setTextWrap(boolean wrapX, boolean wrapY);
    void setFont(const GFXfont *f);
    void setTextSize(uint8_t size);
    int16_t getTextWidth(const char *string);
    
    void drawHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
    void drawVLine(int32_t x, int32_t y, int32_t w, uint32_t color);


    
    

    void refresh();
    
};




#endif
