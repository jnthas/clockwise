#pragma once

#include <Arduino.h>
#include "Locator.h"


struct ImageUtils {


    static void flipHorizontally(uint16_t *image, uint8_t width, uint8_t height) {
      //13x16

      // 0*13 - 0 1 2 3 4 5 6 7 8 9 0 1 2 
      // 1*13 - 3 4 5 6 7 8 9 0 1 2 3 4 5

      int idx = 0;
      uint16_t pixel_temp;

      for (uint8_t h = 0; h < height; h++) {
        idx = h*width;
        for (uint8_t w = 0; w < width/2; w++) {
          pixel_temp = image[idx+w];
          image[idx+w] = image[idx-w+(width-1)];
          image[idx-w+(width-1)] = pixel_temp;
        }
      }     
    }


    static void flipHorizontallyClone(const uint16_t *image, uint16_t *dest, uint8_t width, uint8_t height) {
      //13x16

      // 0*13 - 0 1 2 3 4 5 6 7 8 9 0 1 2 
      // 1*13 - 3 4 5 6 7 8 9 0 1 2 3 4 5

      int idx = 0;

      for (uint8_t h = 0; h < height; h++) {
        idx = h*width;
        for (uint8_t w = 0; w < width; w++) {
          dest[idx+w] = image[idx-w+(width-1)];
        }
      }     
    }

    static void clone(uint16_t* src, uint16_t* dst, int len) {
      memcpy(dst, src, sizeof(src[0])*len);
    }

    static void drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height, uint16_t maskColor) {
      // Draw bitmap pixel by pixel, skipping mask pixels (TRANSPARENT)
      const unsigned short TRANSPARENT = 0xFEFE;
      for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
          uint16_t pixel = pgm_read_word(&bitmap[j * width + i]);
          if (pixel != TRANSPARENT) {  // Only draw non-transparent pixels
            Locator::getDisplay()->drawPixel(x + i, y + j, pixel);
          } else {
            // Optionally, fill with maskColor if you want to erase background
            // Locator::getDisplay()->drawPixel(x + i, y + j, maskColor);
          }
        }
      }
  }    
};
