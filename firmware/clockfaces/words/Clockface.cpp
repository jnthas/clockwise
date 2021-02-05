
#include "Clockface.h"

const char* FORMAT_TWO_DIGITS = "%02d";

EventBus eventBus;

unsigned long lastMillis = 0;

char hInWords[20];
char mInWords[20]; 
char formattedDate[20];

int temperature = 26;

DateI18nPT i18n;

Clockface::Clockface(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void Clockface::setup(DateTime *dateTime) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setTextWrap(true, true);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);  

  updateTime();
  updateDate();
  updateTemperature();
}


void Clockface::update() 
{  
  if (millis() - lastMillis >= 1000) {

    if (_dateTime->getSecond() == 0) {
      updateTime();
    }

    if (_dateTime->getMinute() == 0 && _dateTime->getSecond() == 0) {
      updateDate();    
    }

    if (_dateTime->getMinute() % 15 == 0 && _dateTime->getSecond() == 0) {
      updateTemperature();
    }
    
    lastMillis = millis();
  }  
}

void Clockface::updateTime() {

  Locator::getDisplay()->fillRect(0, 0, 64, 40, 0x0000);  

  i18n.timeInWords(_dateTime->getHour(), _dateTime->getMinute(), hInWords, mInWords);  
  //timeInWords(5, 6, hInWords, mInWords);  

  // Hour
  Locator::getDisplay()->setFont(&hour8pt7b);  
  Locator::getDisplay()->setCursor(1, 14);
  Locator::getDisplay()->setTextColor(0x2589);
  Locator::getDisplay()->println(hInWords);
  
  // Minute
  Locator::getDisplay()->setFont(&minute7pt7b);
  Locator::getDisplay()->setCursor(0, 25);
  Locator::getDisplay()->setTextColor(0xffff);
  Locator::getDisplay()->println(mInWords);

  // Separator line
  Locator::getDisplay()->drawHLine(1, 40, 62, 0xffff);
}


void Clockface::updateDate() {  
  Locator::getDisplay()->fillRect(0, 41, 46, 13, 0x0000);

  // Date
  Locator::getDisplay()->setFont(&minute7pt7b);
  Locator::getDisplay()->setCursor(0, 52);
  Locator::getDisplay()->setTextColor(0x2589);
    
  const char* fmt = i18n.formatDate(_dateTime->getDay(), _dateTime->getMonth());

  Locator::getDisplay()->print(fmt);

  const int dateWidth = Locator::getDisplay()->getTextWidth(fmt);

  // Weekday
  Locator::getDisplay()->setFont(&small4pt7b);
  //Locator::getDisplay()->setFont(&minute7pt7b);
  Locator::getDisplay()->setCursor(dateWidth + 2, 52);
  Locator::getDisplay()->setTextColor(0xffff);
  Locator::getDisplay()->println(i18n.weekDayName(_dateTime->getWeekday()));  
}


void Clockface::updateTemperature() {

  Locator::getDisplay()->fillRect(46, 41, 18, 13, 0x0000);
  Locator::getDisplay()->setFont(&minute7pt7b);

  // Temperature
  // TODO get temperature
  temperature++;
  if (temperature > 30) temperature = 20;

  char buffer[4];  
  sprintf(buffer, "%d~", temperature);
   
  const int tempWidth = Locator::getDisplay()->getTextWidth(buffer);
 
  Locator::getDisplay()->setCursor(62-tempWidth, 52);
  Locator::getDisplay()->setTextColor(0xffff);
  Locator::getDisplay()->println(buffer);


  Locator::getDisplay()->draw(WIFI, 1, 55, 8, 8);
  Locator::getDisplay()->draw(MAIL, 12, 55, 8, 8);
  Locator::getDisplay()->draw(WEATHER_CLOUDY_SUN, 55, 55, 8, 8);
}
