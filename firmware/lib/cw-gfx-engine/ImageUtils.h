#pragma once

#include <Arduino.h>


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
};
