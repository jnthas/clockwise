
#include "Clockface.h"

EventBus eventBus;

const char* FORMAT_TWO_DIGITS = "%02d";


// Graphical
Tile ground(GROUND, 8, 8); 

Object bush(BUSH, 21, 9);
Object cloud1(CLOUD1, 13, 12);
Object cloud2(CLOUD2, 13, 12);
Object hill(HILL, 20, 22);


Mario mario(23, 40);
Block hourBlock(13, 8);
Block minuteBlock(32, 8);


unsigned long lastMillis = 0;


Clockface::Clockface(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void Clockface::setup(DateTime *dateTime) {
  _dateTime = dateTime;

  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, SKY_COLOR);

  ground.fillRow(DISPLAY_HEIGHT - ground._height);

  bush.draw(43, 47);
  hill.draw(0, 34);
  cloud1.draw(0, 21);
  cloud2.draw(51, 7);

  updateTime();


  hourBlock.init();
  minuteBlock.init();
  mario.init();
}

void Clockface::update() {
  hourBlock.update();
  minuteBlock.update();
  mario.update();

  if (_dateTime->getSecond() == 0 && millis() - lastMillis > 1000) {
    mario.jump();
    updateTime();
    lastMillis = millis();

    Serial.println(_dateTime->getFormattedTime());
  }
}

void Clockface::updateTime() {
  hourBlock.setText(String(_dateTime->getHour()));
  minuteBlock.setText(String(_dateTime->getMinute(FORMAT_TWO_DIGITS)));
}