#include "Locator.h"

Adafruit_GFX* Locator::_display;
EventBus* Locator::_eventBus;

void Locator::provide(Adafruit_GFX* display)
{
  _display = display;
}

void Locator::provide(EventBus* eventBus)
{
  _eventBus = eventBus;
}

Adafruit_GFX* Locator::getDisplay() 
{ 
  return _display; 
}

EventBus* Locator::getEventBus() 
{ 
  return _eventBus; 
}
