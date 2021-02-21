#include "DeviceManager.hpp"

namespace DeviceManager
{
    Clockface clockface;

    void setup() 
    {        
        Serial.begin(9600);  

        DeviceStatus::setup();
        WiFiConnection::setup();
        Display::setup();        

        DateTime::setup();
        OTAUpdate::setup();

        clockface.setup();       
        
        delay(1000);
        
        DeviceStatus::setState(DeviceStatus::State::RUNNING);       
    }

    void loop() 
    {
        DeviceStatus::loop();
        OTAUpdate::loop();

        if (DeviceStatus::getCurrentState() == DeviceStatus::State::RUNNING) {
            clockface.update();
        }
    }
    

};