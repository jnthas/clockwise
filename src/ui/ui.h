#ifndef UI_h
#define UI_h

#include <Arduino.h>

#include "../gfx/Super_Mario_Bros__24pt7b.h"

#include <Display.h>
#include <Tile.h>
#include <Locator.h>
#include <Game.h>
#include <Object.h>
#include <ImageUtils.h>
#include "../gfx/elems.h"
#include "mario.h"
#include "block.h"


class UI {
  private:
    Display* _display;

  public:
    UI(Display* display);
    void init();
    void update();
};




#endif