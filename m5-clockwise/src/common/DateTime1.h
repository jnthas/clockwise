
// #ifndef DateTimeCf_h
// #define DateTimeCf_h

// #include <Arduino.h>

// #include <NTPClient.h>
// #include <WiFi.h>

// WiFiUDP udp;
// NTPClient ntp(udp, "ntp-d.nist.gov", -3 * 3600, 3600000);

// struct DateTime {

//   void begin()
//   {
//     ntp.begin();
//     ntp.forceUpdate();
//   }

//   void update()
//   {
//     ntp.update();
//   }

//   String getFormattedTime()
//   {
//     return ntp.getFormattedTime();
//   }

//   char* getHour(const char *format)
//   {
//     static char buffer[3] = {'\0'};
//     snprintf(buffer, sizeof(buffer), format, ntp.getHours());
//     return buffer;
//   }

//   int getHour()
//   {
//     return ntp.getHours();
//   }


//   char* getMinute(const char *format)
//   {
//     static char buffer[3] = {'\0'};
//     snprintf(buffer, sizeof(buffer), format, ntp.getMinutes());
//     return buffer;
//   }

//   int getMinute()
//   {
//     return ntp.getMinutes();
//   }

//   char* getSeconds(const char *format)
//   {
//     static char buffer[3] = {'\0'};
//     snprintf(buffer, sizeof(buffer), format, ntp.getSeconds());
//     return buffer;
//   }

//   int getSeconds() {
//     return ntp.getSeconds();
//   }

// };

// #endif