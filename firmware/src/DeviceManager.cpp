#include "DeviceManager.hpp"

namespace DeviceManager
{
    Clockface clockface;

    void setup() 
    {
        DeviceStatus::setup();
        Display::setup();        
        WiFiConnection::connect();
        DateTime::setup();
        Display::println("Done.");
        delay(1000);

        clockface.setup();       
        DeviceStatus::setStatus(DeviceStatus::State::RUNNING);       
    }

    void loop() 
    {
        DeviceStatus::loop();
        clockface.update();        
    }

}