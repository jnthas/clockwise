#include "Locator.h"

Display* Locator::_display;
EventBus* Locator::_eventBus;

void Locator::provide(Display* display)
{
  _display = display;
}

void Locator::provide(EventBus* eventBus)
{
  _eventBus = eventBus;
}

Display* Locator::getDisplay() 
{ 
  return _display; 
}

EventBus* Locator::getEventBus() 
{ 
  return _eventBus; 
}
