#pragma once

#include <Arduino.h>

class Sprite {

  protected:
    int8_t _x = 0;
    int8_t _y = 0;
    uint8_t _width = 0;
    uint8_t _height = 0;

  public:
    boolean collidedWith(Sprite* sprite);
    void logPosition();

    virtual const char* name();
};
