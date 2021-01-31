
#include <Arduino.h>
#include <M5Stack.h>

#include "common/WiFiConnect.h"
#include "common/DateTime.h"

#include "worldclock/Clockface.h"


Display engine_display(&M5.Lcd);
Clockface clockface(&engine_display);

WiFiConnect wifi;
DateTime dateTime;


// the setup routine runs once when M5Stack starts up
void setup() {
  
  Serial.begin(9600);
  
  wifi.connect();
  dateTime.begin();


  delay(100);

  // Initialize the M5Stack object
  M5.begin(true, false, false, false);

  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();
 
  clockface.setup(&dateTime);  

  M5.update();


  
  //M5.Lcd.qrcode("http://www.m5stack.com",50,10,220,6);
}

// the loop routine runs over and over again forever
void loop() {
  M5.update();
  clockface.update();

}
