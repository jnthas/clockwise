#pragma once

#include <Arduino.h>
#include "Sprite.h"

enum EventType {
  MOVE, 
  COLLISION,
  COLLISION_JUMP,  // New event type for collision-based jumps (no time update)
  SKY_COLOR_CHANGED // Broadcast when sky color changes
};

class EventTask {
  public:
    virtual void execute(EventType event, Sprite* caller, uint16_t value = 0) = 0;
};



