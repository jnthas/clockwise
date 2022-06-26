#pragma once
#include "Locator.h"

struct Object {
  const unsigned short *_image;
  int _width;
  int _height;

  Object(const unsigned short *image, int width, int height) {
    this->_image = image;
    this->_width = width;
    this->_height = height;
  }
  
  void draw(int x, int y) {
    Locator::getDisplay()->drawRGBBitmap(x, y, _image, _width, _height);  
  }
};
