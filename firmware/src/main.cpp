#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Clockface
#include <Clockface.h>
// Commons
#include <WiFiController.h>
#include <CWDateTime.h>
#include <CWPreferences.h>
#include <CWWebServer.h>
#include <CWMcp.h>
#include <StatusController.h>

#define MIN_BRIGHT_DISPLAY_ON 4
#define MIN_BRIGHT_DISPLAY_OFF 0

#define ESP32_LED_BUILTIN 2

MatrixPanel_I2S_DMA *dma_display = nullptr;

Clockface *clockface;

WiFiController wifi;
CWDateTime cwDateTime;

bool autoBrightEnabled;
uint32_t autoBrightMillis = 0;
uint8_t currentBrightSlot = -1;

bool isValidI2SSpeed(uint32_t speed) {
  return speed == 8000000 || speed == 16000000 || speed == 20000000;
}

bool isValidDriver(uint32_t drv) {
  return drv >= 0 && drv <= 5;
}



void displaySetup(bool swapBlueGreen, bool swapBlueRed, uint8_t displayBright, uint8_t displayRotation, uint8_t driver, uint32_t i2cSpeed, uint8_t E_pin)
{
  HUB75_I2S_CFG mxconfig(64, 64, 1);

  if (swapBlueGreen)
  {
    // Swap Blue and Green pins because the panel is RBG instead of RGB.
    mxconfig.gpio.b1 = 26;
    mxconfig.gpio.b2 = 12;
    mxconfig.gpio.g1 = 27;
    mxconfig.gpio.g2 = 13;
  }

  if (swapBlueRed)
  {
    // Swap Blue and Red pins. 
    mxconfig.gpio.b1 = 25;
    mxconfig.gpio.b2 = 14;
    mxconfig.gpio.r1 = 27;
    mxconfig.gpio.r2 = 13;
  }

  mxconfig.gpio.e = E_pin;
  mxconfig.clkphase = false;

  if (isValidDriver(driver)) {
    mxconfig.driver = static_cast<HUB75_I2S_CFG::shift_driver>(driver);
  } else {
    Serial.printf("[ERROR] Invalid driver from config:%d\n", driver);
  }
  if (isValidI2SSpeed(i2cSpeed)) {
    mxconfig.i2sspeed = static_cast<HUB75_I2S_CFG::clk_speed>(i2cSpeed);
  } else {
    Serial.printf("[ERROR] Invalid I2S speed from config:%d\n", i2cSpeed);
  }

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(displayBright);
  dma_display->clearScreen();
  dma_display->setRotation(displayRotation);
}

void refreshAutoBrightness()
{
  ClockwiseParams* params = ClockwiseParams::getInstance();
  autoBrightEnabled = params->autoBrightMax > params->autoBrightMin;
  currentBrightSlot = 255; // Force the next sensor sample to apply its result.
  autoBrightMillis = millis() - 3001U;
  pinMode(params->ldrPin, INPUT);
  if (!autoBrightEnabled)
    dma_display->setBrightness8(params->displayBright);
}

void automaticBrightControl()
{
  if (autoBrightEnabled) {
    if (millis() - autoBrightMillis > 3000)
    {
      int16_t currentValue = analogRead(ClockwiseParams::getInstance()->ldrPin);

      uint16_t ldrMin = ClockwiseParams::getInstance()->autoBrightMin;
      uint16_t ldrMax = ClockwiseParams::getInstance()->autoBrightMax;

      // Defend against invalid settings left by older firmware or the web UI.
      if (ldrMax == 0 || ldrMax <= ldrMin)
      {
        refreshAutoBrightness();
        return;
      }

      const uint8_t minBright = (currentValue < ldrMin ? MIN_BRIGHT_DISPLAY_OFF : MIN_BRIGHT_DISPLAY_ON);
      uint8_t maxBright = ClockwiseParams::getInstance()->displayBright;

      uint8_t slots = 10; //10 slots
      const int16_t boundedValue = currentValue < ldrMin ? ldrMin : (currentValue > ldrMax ? ldrMax : currentValue);
      uint8_t mapLDR = map(boundedValue, ldrMin, ldrMax, 1, slots);
      uint8_t mapBright = map(mapLDR, 1, slots, minBright, maxBright);

      // Serial.printf("LDR: %d, mapLDR: %d, Bright: %d\n", currentValue, mapLDR, mapBright);
      if(abs(currentBrightSlot - mapLDR ) >= 2 || mapBright == 0){
           dma_display->setBrightness8(mapBright);
           currentBrightSlot=mapLDR;
          //  Serial.printf("setBrightness: %d , Update currentBrightSlot to %d\n", mapBright, currentBrightSlot);
      }
      autoBrightMillis = millis();
    }
  }
}

void setupArduinoOta()
{
  ArduinoOTA.setHostname("clockwise");
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.printf("[OTA] Start updating %s\n", type.c_str());
  });
  ArduinoOTA.onEnd([]() { Serial.println("[OTA] Done, rebooting"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress * 100) / total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error %u\n", error);
  });
  ArduinoOTA.begin();
}

void setup()
{
  Serial.begin(115200);
  pinMode(ESP32_LED_BUILTIN, OUTPUT);

  StatusController::getInstance()->blink_led(5, 100);

  ClockwiseParams::getInstance()->load();

  pinMode(ClockwiseParams::getInstance()->ldrPin, INPUT);

  uint8_t driver = ClockwiseParams::getInstance()->driver;
  uint32_t i2cSpeed = ClockwiseParams::getInstance()->i2cSpeed;
  uint8_t E_pin = ClockwiseParams::getInstance()->E_pin;
  
  displaySetup(ClockwiseParams::getInstance()->swapBlueGreen, ClockwiseParams::getInstance()->swapBlueRed, ClockwiseParams::getInstance()->displayBright, ClockwiseParams::getInstance()->displayRotation, driver, i2cSpeed, E_pin);
  clockface = new Clockface(dma_display);

  // MCP server runtime hooks: local time and immediate brightness apply.
  mcpSetTimeProvider([]() -> String { return cwDateTime.getFormattedTime(); });
  mcpSetBrightnessHandler([](uint8_t level) { dma_display->setBrightness8(level); });

  mcpSetAutoBrightnessHandler(refreshAutoBrightness);
  refreshAutoBrightness();

  StatusController::getInstance()->clockwiseLogo();
  delay(1000);

  StatusController::getInstance()->wifiConnecting();
  wifi.begin(); // non-blocking; provisioning continues in loop()
}

// Runs once, as soon as Wi-Fi is up (stored credentials, Improv or AP portal).
void onNetworkReady()
{
  // Keep the radio awake now that Wi-Fi is initialized: modem sleep buffers
  // AP-side TCP handshakes (SYN) for whole beacon intervals, which stalls
  // the embedded web server. Must run after Wi-Fi init, otherwise
  // esp_wifi_set_ps() fails with ESP_ERR_WIFI_NOT_INIT and the call is lost.
  WiFi.setSleep(false);

  StatusController::getInstance()->ntpConnecting();
  cwDateTime.begin(ClockwiseParams::getInstance()->timeZone.c_str(), 
      ClockwiseParams::getInstance()->use24hFormat, 
      ClockwiseParams::getInstance()->ntpServer.c_str(),
      ClockwiseParams::getInstance()->manualPosix.c_str());
  clockface->setup(&cwDateTime);

  setupArduinoOta();
}

void loop()
{
  cwDateTime.update();

  if (wifi.update()) // also services Improv Serial on every pass
  {
    onNetworkReady();
  }

  if (wifi.isConnected())
  {
    ClockwiseWebServer::getInstance()->handleHttpRequest();
    ArduinoOTA.handle();
    ezt::events();
  }

  if (wifi.connectionSucessfulOnce)
  {
    clockface->update();
  }

  automaticBrightControl();
}
