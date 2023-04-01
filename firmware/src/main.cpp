#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Clockface
#include <Clockface.h>
// Commons
#include <WiFiController.h>
#include <CWDateTime.h>
#include <CWPreferences.h>
#include <CWWebServer.h>
#include <StatusController.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

Clockface *clockface;

WiFiController wifi;
CWDateTime cwDateTime;

void displaySetup(bool swapBlueGreen, uint8_t displayBright)
{
  HUB75_I2S_CFG mxconfig(
      64, // module width
      64, // module height
      1   // Chain length
  );

  if (swapBlueGreen)
  {
    // Swap Blue and Green pins because the panel is RBG instead of RGB.
    mxconfig.gpio.b1 = 26;
    mxconfig.gpio.b2 = 12;
    mxconfig.gpio.g1 = 27;
    mxconfig.gpio.g2 = 13;
  }

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
  pinMode(2, OUTPUT);

  StatusController::getInstance()->blink_led(5, 100);

  wifi.begin();
  Serial.println(ClockwiseParams::getInstance()->swapBlueGreen);
  Serial.println(ClockwiseParams::getInstance()->use24hFormat);
  Serial.println(ClockwiseParams::getInstance()->displayBright);
  Serial.println(ClockwiseParams::getInstance()->timeZone);
  Serial.println(ClockwiseParams::getInstance()->wifiSsid);
  Serial.println(ClockwiseParams::getInstance()->wifiPwd);

  // displaySetup(params.swapBlueGreen, params.displayBright);
  //  clockface = new Clockface(dma_display);

  // io.clockwiseLogo();
  // io.wifiConnecting();
  // wifi.connect();
  // io.ntpConnecting();
  // cwDateTime.begin();

  // clockface->setup(&cwDateTime);
}

void loop()
{
  wifi.handleImprovWiFi();

  if (WiFiController::isConnected())
  {
    ClockwiseWebServer::getInstance()->handleHttpRequest();
  }

  // clockface->update();
}
