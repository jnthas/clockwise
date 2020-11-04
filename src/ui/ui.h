#ifndef UI_h
#define UI_h

#include <Arduino.h>
#include "../engine/Display.h"
#include "../engine/Tile.h"
#include "../engine/Locator.h"
#include "../engine/Game.h"
#include "../engine/Object.h"
#include "../engine/ImageUtils.h"
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