
#include "Clockface.h"

const char* FORMAT_TWO_DIGITS = "%02d";

EventBus eventBus;

unsigned long lastMillis = 0;

char hInWords[20];
char mInWords[20]; 

int temperature = 8;

Clockface::Clockface(Display* display) {
  _display = display;

  Locator::provide(display);
  Locator::provide(&eventBus);
}

void Clockface::setup(DateTime *dateTime) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setTextWrap(true, true);
  ///Locator::getDisplay()->drawRect(0, 0, 65, 65, 0xffff);

  printTime(true);

  Serial.println(_dateTime->getHour(FORMAT_TWO_DIGITS));
  Serial.println(":");
  Serial.println(_dateTime->getMinute(FORMAT_TWO_DIGITS));
  Serial.println(" -> ");
  //Serial.println(printWords(_dateTime->getHour(), _dateTime->getMinute()));



  // Serial.println(printWords(6, 5));
  // Serial.println(printWords(6, 15));
  // Serial.println(printWords(6, 30));
  // Serial.println(printWords(6, 45));
  // Serial.println(printWords(23, 45));

  // Serial.println(printWords(3, 25));
  // Serial.println(printWords(8, 31));
  // Serial.println(printWords(10, 59));
  
  
  // Serial.println(printWords(3, 11));
  // Serial.println(printWords(3, 12));
  // Serial.println(printWords(3, 13));
  // Serial.println(printWords(3, 14));
  // Serial.println(printWords(3, 15));
  // Serial.println(printWords(3, 16));
  // Serial.println(printWords(3, 17));
  // Serial.println(printWords(3, 18));
  // Serial.println(printWords(3, 19));
  // Serial.println(printWords(3, 20));
  // Serial.println(printWords(3, 21));

  // Serial.println(printWords(23, 59));
  // Serial.println(printWords(0, 0));
  // Serial.println(printWords(0, 1));
  
  // Serial.println(printWords(11, 59));
  // Serial.println(printWords(12, 0));
  // Serial.println(printWords(12, 1));

  // Serial.println(printWords(13, 0));
}

void Clockface::update() 
{  
  if (_dateTime->getSecond() == 0 && millis() - lastMillis > 1000) {
    printTime(false);
    lastMillis = millis();
  }
}

void Clockface::printTime(boolean initialSetup) {

  if (initialSetup) {
    Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);  
  } else {
    Locator::getDisplay()->fillRect(0, 0, 64, 39, 0x0000);  
  }
  timeInWords(_dateTime->getHour(), _dateTime->getMinute(), hInWords, mInWords);  
  //timeInWords(12, 30, hInWords, mInWords);  

  // Hour
  Locator::getDisplay()->setFont(&hour8pt7b);  
  Locator::getDisplay()->setCursor(0, 10);
  Locator::getDisplay()->setTextColor(0x2589);
  Locator::getDisplay()->println(hInWords);
  
  // Minute
  Locator::getDisplay()->setFont(&minute7pt7b);
  Locator::getDisplay()->setCursor(0, 22);
  Locator::getDisplay()->setTextColor(0xffff);
  Locator::getDisplay()->println(mInWords);

  // Separator line
  Locator::getDisplay()->drawHLine(2, 40, 58, 0xffff);

  if (initialSetup || _dateTime->getMinute() == 0) {

    Locator::getDisplay()->fillRect(0, 41, 32, 64, 0x0000);
  
    // Date
    Locator::getDisplay()->setFont(&minute7pt7b);
    Locator::getDisplay()->setCursor(0, 52);
    Locator::getDisplay()->setTextColor(0x2589);
    Locator::getDisplay()->print(String(_dateTime->getDay()));
    Locator::getDisplay()->print("/");
    Locator::getDisplay()->print(String(_dateTime->getMonth()));

    // Weekday
    Locator::getDisplay()->setFont(&small4pt7b);
    Locator::getDisplay()->setCursor(1, 61);
    Locator::getDisplay()->setTextColor(0xffff);
    Locator::getDisplay()->println(_dateTime->getWeekdayName());
  }

  if (initialSetup || _dateTime->getMinute() % 15 == 0) {
    Locator::getDisplay()->fillRect(32, 41, 32, 64, 0x0000);

    // Temperature
    // TODO get temperature
    temperature++;
    Locator::getDisplay()->setFont(&minute7pt7b);
    Locator::getDisplay()->setCursor(temperature < 10 ? 52 : 46, 52);
    Locator::getDisplay()->setTextColor(0xffff);
    Locator::getDisplay()->println(String(temperature));
    Locator::getDisplay()->drawCircle(59, 46, 1, 0xffff);
  }
 

  // Serial.println("Hour:");
  // Serial.println(hInWords);
  // Serial.println("Min :");
  // Serial.println(mInWords);
}


// void Clockface::timeInWords(int h, int m, char* hWords, char* mWords)
// { 

//     int hour12 = (h % 12);
//     if (h == 12) {
//       hour12 = h;
//     }

//     char hours[][13] = { 
//                          "zero", "uma", "duas", "tres", "quatro", 
//                          "cinco", "seis", "sete", "oito", "nove", 
//                          "dez", "onze", "doze"
//                        }; 

//     char min_dec[][10] = {
//                           "zero", "dez", "vinte", "trinta", "quarenta", "cinquenta"
//                         };
    
//     char min_uni[][19] = { 
//                             "zero", "um", "dois", "tres", "quatro", "cinco", 
//                             "seis", "sete", "oito", "nove", "dez", 
//                             "onze", "doze", "treze", "quatorze", "quinze", 
//                             "dezesseis", "dezessete", "dezoito", "dezenove"
//                          };
        

//     if (h == 0 && m == 0) {
//       strcpy(hWords, "meia\nnoite");        
//       strcpy(mWords, "");
//     } else if (h == 12 && m == 0) {
//       strcpy(hWords, "meio\ndia");        
//       strcpy(mWords, "");
//     } else if (m == 0) {      
//       strcpy(hWords, hours[hour12]); 
//       strcat(hWords, "\nhora");
      
//       if (hour12 > 1) {
//         strcat(hWords, "s");
//       }

//       strcpy(mWords, "");
//     } else if (m == 30) {      
//       strcpy(hWords, hours[hour12]); 
//       if (h == 0 || h == 12) {
//         strcpy(mWords, "trinta");
//       } else {
//         strcpy(mWords, "e meia");
//       }
      
//     } else {
//       strcpy(hWords, hours[hour12]);

//       if (m < 20) {
//         strcpy(mWords, min_uni[m]);
//       } else {
//         strcpy(mWords, min_dec[(m - (m%10))/10]); 
        
//         if (m%10 > 0) {
//           strcat(mWords, " e\n"); 
//           strcat(mWords, min_uni[m%10]); 
//         }
        
//       }
      
//     }
  
// }



void Clockface::timeInWords(int h, int m, char* hWords, char* mWords)
{ 

    int hour12 = (h % 12);
    if (h == 12) {
      hour12 = h;
    }

    char hours[][13] = { 
                         "zero", "one", "two", "three", "four", 
                         "five", "six", "seven", "eight", "nine", 
                         "ten", "eleven", "twelve"
                       }; 

    char min_dec[][10] = {
                          "zero", "ten", "twenty", "thirty", "forty", "fifty"
                        };
    
    char min_uni[][19] = { 
                            "zero", "one", "two", "three", "four", "five", 
                            "six", "seven", "eight", "nine", "ten", 
                            "eleven", "twelve", "thirteen", "fourteen", "fifteen", 
                            "sixteen", "seventeen", "eighteen", "nineteen"
                         };
        

    if (h == 0 && m == 0) {
      strcpy(hWords, "mid\nnight");        
      strcpy(mWords, "");
    } else if (h == 12 && m == 0) {
      strcpy(hWords, "mid\nday");        
      strcpy(mWords, "");
    } else if (m == 0) {      
      strcpy(hWords, hours[hour12]); 
      strcpy(mWords, "o'clock");
    } else if (m == 30) {      
      strcpy(hWords, hours[hour12]); 
      if (h == 0 || h == 12) {
        strcpy(mWords, "thirty");
      } else {
        strcpy(mWords, "a half");
      }
      
    } else {
      strcpy(hWords, hours[hour12]);

      if (m < 20) {
        strcpy(mWords, min_uni[m]);
      } else {
        strcpy(mWords, min_dec[(m - (m%10))/10]); 
        
        if (m%10 > 0) {
          strcat(mWords, "\n"); 
          strcat(mWords, min_uni[m%10]); 
        }
        
      }
      
    }
  
}




// String Clockface::printWords(int h, int m)
// { 
//     char nums[][64] = { "zero", "one", "two", "three", "four", 
//                         "five", "six", "seven", "eight", "nine", 
//                         "ten", "eleven", "twelve", "thirteen", 
//                         "fourteen", "fifteen", "sixteen", "seventeen", 
//                         "eighteen", "nineteen", "twenty", "twenty one", 
//                         "twenty two", "twenty three", "twenty four", 
//                         "twenty five", "twenty six", "twenty seven", 
//                         "twenty eight", "twenty nine", 
//                       }; 
    
//     if (m == 0) 
//         return String(nums[h]) + String(" o' clock"); 
  
//     else if (m == 1) 
//         return String("one minute past ") + String(nums[h]); 
  
//     else if (m == 59) 
//         return String("one minute to ") + String(nums[(h % 12) + 1]); 
  
//     else if (m == 15) 
//         return String("quarter past ") + String(nums[h]); 
  
//     else if (m == 30) 
//         return String("half past ") + String(nums[h]); 
  
//     else if (m == 45) 
//         return String("quarter to ") + String(nums[(h % 12) + 1]); 
  
//     else if (m <= 30) 
//         return String(nums[m]) + String(" minutes past ") + String(nums[h]); 
  
//     else if (m > 30) 
//         return String(nums[60 - m]) + String(" minutes to ") + String(nums[(h % 12) + 1]); 
// }