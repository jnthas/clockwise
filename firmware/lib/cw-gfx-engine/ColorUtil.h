#pragma once
#include <Arduino.h>
#include "Macros.h"


const struct 
{ 
   static uint16_t adjustBright(uint16_t color, uint8_t bright) {

        uint8_t r = ((color >> 11) & 0x1F);
        uint8_t g = ((color >> 5) & 0x3F);
        uint8_t b = (color & 0x1F);


        uint8_t newRed   = trunc(r + bright);
        uint8_t newGreen = trunc(g + bright);
        uint8_t newBlue  = trunc(b + bright);

        return color565(newRed, newGreen, newBlue);
   }

   static uint16_t brighter(uint16_t color, uint8_t factor) {
        
        uint8_t r = ((color >> 11) & 0x1F);
        uint8_t g = ((color >> 5) & 0x3F);
        uint8_t b = (color & 0x1F);

        uint8_t newRed   = MIN(trunc(r * (factor / 10)), 255);
        uint8_t newGreen = MIN(trunc(g * (factor / 10)), 255);
        uint8_t newBlue  = MIN(trunc(b * (factor / 10)), 255);

        
        // Serial.printf("Old RGB %d %d %d\n", r, g, b);
        // Serial.printf("New RGB %d %d %d\n", newRed, newGreen, newBlue);
        // delay(1000);



        return color565(newRed, newGreen, newBlue);

   }

    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
} COLOR_UTIL;
