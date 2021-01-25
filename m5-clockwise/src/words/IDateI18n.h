#ifndef IDateI18n_h
#define IDateI18n_h

#include <Arduino.h>

class IDateI18n {
    
    virtual void formatDate(int day, int month, char* formattedDate);
    virtual const char* weekDayName(int weekday);
    virtual void timeInWords(int h, int m, char* hWords, char* mWords);
};

#endif