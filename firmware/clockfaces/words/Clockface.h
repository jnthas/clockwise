#ifndef WordsClockFace_h
#define WordsClockFace_h

#include <Arduino.h>

#include <Display.h>
#include <Tile.h>
#include <Locator.h>
#include <Game.h>
#include <Object.h>
#include <ImageUtils.h>

#include "../common/IClockface.h"

#include "hour8pt7b.h"
#include "minute7pt7b.h"
#include "small4pt7b.h"
#include "DateI18nPT.h"
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