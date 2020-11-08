#ifndef IClockface_h
#define IClockface_h

#include "DateTime.h"

class IClockface {

    //virtual void setup(DateTime *dateTime) = 0;
    virtual void setup(DateTime *dateTime) = 0;
    virtual void update() = 0;

};

#endif