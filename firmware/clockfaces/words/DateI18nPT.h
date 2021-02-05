#ifndef IDateI18nPT_h
#define IDateI18nPT_h


#include <Arduino.h>
#include "IDateI18n.h"


class DateI18nPT: public IDateI18n 
{
  private:
    char buffer[5];
    char weekDayTemp[4]="";
    const char* _weekDayWords = "DOM\0SEG\0TER\0QUA\0QUI\0SEX\0SAB\0";

  public:
    virtual const char* formatDate(int day, int month) {
      sprintf(buffer, "%d/%d", day, month);
      return buffer;
    }

    virtual const char* weekDayName(int weekday) {
      strncpy(weekDayTemp, _weekDayWords + (weekday*4), 4);
      return weekDayTemp;
    }

    virtual void timeInWords(int h, int m, char* hWords, char* mWords) {

      int hour12 = (h % 12);

      if (h == 12) {
        hour12 = h;
      }

      char hours[][13] = { 
                          "zero", "uma", "duas", "tres", "quatro", 
                          "cinco", "seis", "sete", "oito", "nove", 
                          "dez", "onze", "doze"
                        }; 

      char min_dec[][10] = {
                            "zero", "dez", "vinte", "trinta", "quarenta", "cinquenta"
                          };
      
      char min_uni[][19] = { 
                            "zero", "um", "dois", "tres", "quatro", "cinco", 
                            "seis", "sete", "oito", "nove", "dez", 
                            "onze", "doze", "treze", "quatorze", "quinze", 
                            "dezesseis", "dezessete", "dezoito", "dezenove"
                          };
          

      if (h == 0 && m == 0) {
        strcpy(hWords, "meia\nnoite");        
        strcpy(mWords, "");
      } else if (h == 12 && m == 0) {
        strcpy(hWords, "meio\ndia");        
        strcpy(mWords, "");
      } else if (m == 0) {      
        strcpy(hWords, hours[hour12]); 
        strcat(hWords, "\nhora");
        
        if (hour12 > 1) {
          strcat(hWords, "s");
        }

        strcpy(mWords, "");
      } else if (m == 30) {      
        strcpy(hWords, hours[hour12]); 
        if (h == 0 || h == 12) {
          strcpy(mWords, "trinta");
        } else {
          strcpy(mWords, "e meia");
        }
        
      } else {
        strcpy(hWords, hours[hour12]);

        if (m < 20) {
          strcpy(mWords, min_uni[m]);
        } else {
          strcpy(mWords, min_dec[(m - (m%10))/10]); 
          
          if (m%10 > 0) {
            strcat(mWords, "\ne "); 
            strcat(mWords, min_uni[m%10]); 
          }
          
        }
        
      }

    }
};

#endif