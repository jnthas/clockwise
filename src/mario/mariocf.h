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
#include "../Clockface.h"


class MarioClockface: public Clockface {
  private:
    Display* _display;

  public:
    MarioClockface(Display* display);
    void setup();
    void update();
};

#endif