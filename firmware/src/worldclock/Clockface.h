#ifndef WorldClockClockFace_h
#define WorldClockClockFace_h

#include <Arduino.h>

#include <Display.h>
#include <Tile.h>
#include <Locator.h>
#include <Game.h>
#include <Object.h>
#include <ImageUtils.h>
#include <Scroll.h>
#include <ColorUtil.h>

#include "../common/IClockface.h"

#include "small4pt7b.h"
#include "worldmap.h"
#include "worldmap_mask.h"

#include "../common/Icons.h"


class Clockface: public IClockface {
  private:
    Display* _display;
    DateTime* _dateTime;

    void timeInWords(int h, int m, char* hWords, char* mWords);
    void updateTime();
    void updateDate();
    void updateTemperature();

  public:
    Clockface(Display* display);
    void setup(DateTime *dateTime);
    void update();
};

#endif