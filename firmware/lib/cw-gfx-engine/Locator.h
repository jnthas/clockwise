#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "EventBus.h"

class Locator {
  private: 
    static Adafruit_GFX* _display;
    static EventBus* _eventBus;

  public:    
    static Adafruit_GFX* getDisplay();
    static EventBus* getEventBus();
    static void provide(Adafruit_GFX* display);
    static void provide(EventBus* eventBus);
};
