
#include "Clockface.h"

const char* FORMAT_TWO_DIGITS = "%02d";

EventBus eventBus;

unsigned long lastMillis = 0;
unsigned short x = 0;


Scroll _scroll(0, 0, 108, 54);    


Clockface::Clockface(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void Clockface::setup(DateTime *dateTime) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setTextWrap(true, true);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);  
}

void Clockface::update() 
{  
  if (millis() - lastMillis >= 100) {
   
    _scroll.horizontalScroll(_WORLD_MAP, _WORLDMAP_MASK, _display, 1);
      
    Locator::getDisplay()->fillRect(0, 55, 64, 9, 0x0000);  

    Locator::getDisplay()->setFont(&small4pt7b);  
    Locator::getDisplay()->setTextColor(0xffff);    
    Locator::getDisplay()->setCursor(1, 62);    
    Locator::getDisplay()->println(this->_dateTime->getFormattedTime());

    lastMillis = millis();
  }  
}

