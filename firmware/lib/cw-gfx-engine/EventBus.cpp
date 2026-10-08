
#include "EventBus.h"

void EventBus::broadcast(EventType event, Sprite* sender) {
  broadcast(event, sender, 0);
}

void EventBus::broadcast(EventType event, Sprite* sender, uint16_t value) {
  for (uint8_t i = 0; i < _subNum; i++) {
    _subscriptions[i]->execute(event, sender, value);
  }
}


void EventBus::subscribe(EventTask* task) {
  
  if (_subNum < 5) {
    _subscriptions[_subNum] = task;
    _subNum++;
  } else {
    Serial.println("Out of space");
  }

}
