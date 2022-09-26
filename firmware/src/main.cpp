#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Clockface
#include <Clockface.h>
// Commons
#include <WiFiConnect.h>
#include <CWDateTime.h>

#include <IOManager.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myYELLOW = dma_display->color565(255, 255, 0);
uint16_t myCYAN = dma_display->color565(0, 255, 255);
uint16_t myMAGENTA = dma_display->color565(255, 0, 255);

uint16_t myCOLORS[8] = {myRED, myGREEN, myBLUE, myWHITE, myYELLOW, myCYAN, myMAGENTA, myBLACK};

Clockface *clockface;

WiFiConnect wifi;
CWDateTime cwDateTime;
IOManager io;

byte displayBright = 32;

void displaySetup() 
{
  HUB75_I2S_CFG mxconfig(
    64,   // module width
    64,   // module height
    1    // Chain length
  );

#if HUB75_BLUE_GREEN_SWAP
  // Swap Blue and Green pins because the panel is RBG instead of RGB.
  mxconfig.gpio.b1 = 26;
  mxconfig.gpio.b2 = 12;
  mxconfig.gpio.g1 = 27;
  mxconfig.gpio.g2 = 13;
#endif

  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(displayBright);
  dma_display->clearScreen();
}

void setup()
{
  Serial.begin(115200);

  displaySetup();
  clockface = new Clockface(dma_display);

  io.clockwiseLogo();
  io.wifiConnecting();
  wifi.connect();
  io.ntpConnecting();
  cwDateTime.begin();
  
  clockface->setup(&cwDateTime);
}

void loop()
{
  clockface->update();
}
