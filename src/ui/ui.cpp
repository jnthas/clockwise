
#include "ui.h"

uint8_t hour = 23;
uint8_t minute = 0;

EventBus eventBus;

Tile ground(GROUND, 8, 8); 

Object bush(BUSH, 21, 9);
Object cloud1(CLOUD1, 13, 12);
Object cloud2(CLOUD2, 13, 12);
Object hill(HILL, 20, 22);


Mario mario(30, 40);
Block hourBlock(13, 8);
Block minuteBlock(32, 8);


unsigned long lastMillis = 0;


UI::UI(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void UI::init() {
  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, SKY_COLOR);
  

  ground.fillRow(DISPLAY_HEIGHT - ground._height);

  bush.draw(43, 47);
  hill.draw(0, 34);
  cloud1.draw(0, 21);
  cloud2.draw(51, 7);

  String strMin = String(minute);    
  hourBlock.setText(String(hour));
  minuteBlock.setText((minute < 10 ? "0" : "") + strMin);
    
  hourBlock.init();
  minuteBlock.init();

  mario.init();
}

void UI::update() {
  hourBlock.update();
  minuteBlock.update();
  
  mario.update();
    


  // uint16_t test[] = {0,1,2,3,4,5,6,7,8,9,1,2};

  // for (int i = 0; i < 12; i++) {
  //   Serial.print(test[i]);
  // }

  // ImageUtils::flipHorizontally(test, 4, 3);
  // Serial.println("");

  // for (int i = 0; i < 12; i++) {
  //   Serial.print(test[i]);
  // }

  // Serial.println("");

  



  if (millis() - lastMillis > 3000) {
    mario.jump();

    
    minute++;
    if (minute >= 60) {
      hour++;
      minute = 0;

      if (hour >= 24) {
        hour = 0;
      }
    }

    String strMin = String(minute);    
    hourBlock.setText(String(hour));
    minuteBlock.setText((minute < 10 ? "0" : "") + strMin);

    lastMillis = millis();
  }


  // if (millis() - lastMillis > 3000) {
  //   Locator::getDisplay()->refresh();
  // }

}

