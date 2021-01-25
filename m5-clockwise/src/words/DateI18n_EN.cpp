#include "DateI18n_EN.h"

const char* _weekDayWords[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

void DateI18n_EN::formatDate(int day, int month, char* formattedDate) {
  String fmt = "";

  fmt += month;
  fmt += "/";
  fmt += day;
  
  fmt.toCharArray(formattedDate, 5);
}

const char* DateI18n_EN::weekDayName(int weekday) {
  return _weekDayWords[weekday];
}

void DateI18n_EN::timeInWords(int h, int m, char* hWords, char* mWords) {
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
    strcpy(hWords, "noon");
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
    
    if (m < 10) {
      strcpy(mWords, "oh\n");
      strcat(mWords, min_uni[m]); 

    } else if (m < 20) {
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
