
#include "Clockface.h"

const char* FORMAT_TWO_DIGITS = "%02d";

EventBus eventBus;

unsigned long lastMillis = 0;
unsigned short x = 0;


Scroll _scroll(0, 0, 108, 54);    


Clockface::Clockface(Display* display) {
  _display = display;

  ///this->_scroll->horizontalScroll(1);

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void Clockface::setup(DateTime *dateTime) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setTextWrap(true, true);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);  
  ///Locator::getDisplay()->draw(_WORLD_MAP, 0, 0, 108, 54);
}

void Clockface::update() 
{  
  if (millis() - lastMillis >= 100) {

    //Serial.println(this->_dateTime->getFormattedTime());
    
    _scroll.horizontalScroll(_WORLD_MAP, _display, 1);
        
    lastMillis = millis();
  }  
}
