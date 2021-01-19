#ifndef Locator_h
#define Locator_h

#include <Arduino.h>
#include "Display.h"
#include "EventBus.h"

class Locator {
  private: 
    static Display* _display;
    static EventBus* _eventBus;

  public:    
    static Display* getDisplay();
    static EventBus* getEventBus();
    static void provide(Display* display);
    static void provide(EventBus* eventBus);
};


#endif