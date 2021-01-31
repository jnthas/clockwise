#include "Scroll.h"

Scroll::Scroll(int startX, int startY, int w, int h) 
{   
    this->_startX = startX;
    this->_startY = startY;

    this->_anchorX = startX;
    this->_anchorY = startY;

    this->_width = w;
    this->_height = h;
}

void Scroll::horizontalScroll(const unsigned short frame[], Display* display, int step)
{ 
  _anchorX++;

  unsigned int limit = 64;

  if (_anchorX + 64 > _width) {
    limit = _width - _anchorX;
  }

  if (limit == 0) {
    _anchorX = 0;
  }

  Serial.println(limit);

  display->croppedDraw(frame, 0, 0, _anchorX, _anchorY, limit, _height, _width, _height);

  if (limit < 64) {

    display->croppedDraw(frame, limit+1, 0, 0, _anchorY, 64-limit, _height, _width, _height);
  }
  

}
