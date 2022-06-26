#pragma once

#include <Arduino.h>
#include "EventTask.h"
#include "Sprite.h"

class EventBus {
  private:
    EventTask* _subscriptions[5];
    uint8_t _subNum = 0;

  public:
    void broadcast(EventType event, Sprite* sender);
    void subscribe(EventTask* task);
    
};


