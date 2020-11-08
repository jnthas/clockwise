#ifndef WordsClockFace_h
#define WordsClockFace_h

#include <Arduino.h>

#include <Display.h>
#include <Tile.h>
#include <Locator.h>
#include <Game.h>
#include <Object.h>
#include <ImageUtils.h>
#include "../mario/gfx/Super_Mario_Bros__24pt7b.h"
#include "../Clockface.h"


class WordsClockFace: public Clockface {
  private:
    Display* _display;
    String printWords(int h, int m);

  public:
    WordsClockFace(Display* display);
    void setup();
    void update();
};

#endif