#pragma once

#include "CWDateTime.h"

class IClockface {

    //virtual void setup(DateTime *dateTime) = 0;
    virtual void setup(CWDateTime *dateTime) = 0;
    virtual void update() = 0;

};
