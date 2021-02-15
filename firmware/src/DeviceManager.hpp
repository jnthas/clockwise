#pragma once

#include <Arduino.h>
#include <Display.hpp>
#include <WiFiConnection.hpp>
#include <DateTime.hpp>
#include <Clockface.h>

#include "DeviceStatus.hpp"

namespace DeviceManager
{

    /*

    Initialize:

    - Device Status
    - Display      
    - Wifi Manager        
    - Date/Time
    

    - Sensors/Modules (temperature, humidity, rtc, etc)
    - Events (news, messages, weather)
    - Webserver (configuration, clockfaces, i18n, colors)
    - Fonts
    - Watchdog    

    - Clockface
    
    
    */



    void setup();
    void loop();

}