#ifndef MarioClockface_h
#define MarioClockface_h

#include <Arduino.h>

#include "gfx/Super_Mario_Bros__24pt7b.h"

#include <Display.h>
#include <Tile.h>
#include <Locator.h>
#include <Game.h>
#include <Object.h>
#include <ImageUtils.h>
#include "gfx/assets.h"
#include "gfx/mario.h"
#include "gfx/block.h"
#include "../common/IClockface.h"
#include "../common/DateTime.h"


class Clockface: public IClockface {
  private:
    Display* _display;
    DateTime* _dateTime;
    void updateTime();

  public:
    Clockface(Display* display);
    void setup(DateTime *dateTime);
    void update();
};

#endif