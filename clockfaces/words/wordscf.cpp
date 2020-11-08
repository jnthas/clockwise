
#include "wordscf.h"

uint8_t hour = 23;
uint8_t minute = 0;

EventBus eventBus;


unsigned long lastMillis = 0;


WordsClockFace::WordsClockFace(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void WordsClockFace::setup() {
  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);

  
  Locator::getDisplay()->setTextColor(0x000E);       
  Locator::getDisplay()->setCursor(0, 7);


}

void WordsClockFace::update() {
  
  if (millis() - lastMillis > 2000) {

    minute++;
    if (minute >= 60) {
      hour++;
      minute = 0;

      if (hour >= 24) {
        hour = 0;
      }
    }
    
    Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);
    Locator::getDisplay()->setCursor(0, 7);
    Locator::getDisplay()->print(printWords(hour, minute));

    lastMillis = millis();
  }

}




String WordsClockFace::printWords(int h, int m) 
{ 
    char nums[][64] = { "zero", "one", "two", "three", "four", 
                        "five", "six", "seven", "eight", "nine", 
                        "ten", "eleven", "twelve", "thirteen", 
                        "fourteen", "fifteen", "sixteen", "seventeen", 
                        "eighteen", "nineteen", "twenty", "twenty one", 
                        "twenty two", "twenty three", "twenty four", 
                        "twenty five", "twenty six", "twenty seven", 
                        "twenty eight", "twenty nine", 
                      }; 
    
    
    if (m == 0) 
        return String(nums[h]) + String(" o' clock\n"); 
  
    else if (m == 1) 
        return String("one \nminute \npast \n") + String(nums[h]); 
  
    else if (m == 59) 
        return String("one \nminute \nto \n") + String(nums[(h % 12) + 1]); 
  
    else if (m == 15) 
        return String("quarter \npast \n") + String(nums[h]); 
  
    else if (m == 30) 
        return String("half \npast \n") + String(nums[h]); 
  
    else if (m == 45) 
        return String("quarter \nto \n") + String(nums[(h % 12) + 1]); 
  
    else if (m <= 30) 
        return String(nums[m]) + String("\nminutes \npast \n") + String(nums[h]); 
  
    else if (m > 30) 
        return String(nums[60 - m]) + String("\nminutes \nto \n") + String(nums[(h % 12) + 1]); 
} 
  
