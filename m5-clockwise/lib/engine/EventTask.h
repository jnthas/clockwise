
#ifndef EventTask_h
#define EventTask_h

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


#endif



