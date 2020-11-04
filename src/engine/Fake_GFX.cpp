

#include "Fake_GFX.h"

FakeGFX::FakeGFX() {
  for (short x = 0; x < 64; x++) {
    for (short y = 0; y < 64; y++) {
      display[x][y] = false;
    }
  }
}

void FakeGFX::drawPixel(int16_t x, int16_t y, uint16_t color) {  
  display[x][y] = (color != 0);  
}

void FakeGFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (short xx = x; xx < w; xx++) {
    for (short yy = y; yy < h; yy++) {
      display[xx][yy] = false;
    }
  }
}

void FakeGFX::refresh() {
  Serial.println("");
  for (short x = 0; x < 64; x++) {
    for (short y = 0; y < 64; y++) {
      if (display[y][x]) {
        Serial.print("*");
      } else {
        Serial.print(" ");
      }      
      delay(1);
    }
    Serial.print("\n");
  }
}

    


