#pragma once

#ifdef ARDUINO_ESP32_DEV 

#include <Arduino.h>
#include <PxMatrix.h>
#include <Display.h>

namespace DisplayLedMatrix {
    
    void initialize();
    void setup();

}


#endif