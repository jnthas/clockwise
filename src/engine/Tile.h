
#ifndef Tile_h
#define Tile_h

#include "Locator.h"
#include "Game.h"

struct Tile {
  const unsigned short *_image;
  int _width;
  int _height;

  Tile(const unsigned short *image, int width, int height) {
    this->_image = image;
    this->_width = width;
    this->_height = height;
  }
  
  void draw(int x, int y) {
    Locator::getDisplay()->draw(_image, x, y, _width, _height);
  }

  void fillRow(int y) {    
    for (int xx = 0; xx < DISPLAY_WIDTH; xx += _width) {
      draw(xx, y);      
    }    
  }
  

};


#endif