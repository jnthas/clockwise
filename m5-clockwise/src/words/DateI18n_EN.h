#ifndef DateI18n_PT_h
#define DateI18n_PT_h

#include <Arduino.h>
#include "IDateI18n.h"

class DateI18n_EN: public IDateI18n {  
  private:   

  public:
    void formatDate(int day, int month, char* formattedDate);
    const char* weekDayName(int weekday);
    void timeInWords(int h, int m, char* hWords, char* mWords);
};

#endif