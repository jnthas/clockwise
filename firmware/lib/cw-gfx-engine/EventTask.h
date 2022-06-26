#pragma once

#include <Arduino.h>
#include "Sprite.h"

enum EventType {
    MOVE, 
    COLLISION
};

class EventTask {
  public:
    virtual void execute(EventType event, Sprite* caller) = 0;
};



