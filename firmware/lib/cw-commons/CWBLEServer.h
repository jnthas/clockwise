#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "CWPreferences.h"
#include "StatusController.h"

// Service UUIDs
#define DEVICE_CONFIG_SERVICE_UUID  "11111111-0000-1000-8000-00805F9B34FB"
#define NETWORK_SERVICE_UUID        "22222222-0000-1000-8000-00805F9B34FB"
#define DISPLAY_SERVICE_UUID        "33333333-0000-1000-8000-00805F9B34FB"
#define DEVICE_INFO_SERVICE_UUID    "44444444-0000-1000-8000-00805F9B34FB"

// Device Config Characteristics
#define DISPLAY_BRIGHT_UUID         "11111111-0001-1000-8000-00805F9B34FB"
#define AUTO_BRIGHT_MIN_UUID        "11111111-0002-1000-8000-00805F9B34FB"
#define AUTO_BRIGHT_MAX_UUID        "11111111-0003-1000-8000-00805F9B34FB"
#define SWAP_BLUE_GREEN_UUID        "11111111-0004-1000-8000-00805F9B34FB"
#define SWAP_BLUE_RED_UUID          "11111111-0005-1000-8000-00805F9B34FB"
#define USE_24H_FORMAT_UUID         "11111111-0006-1000-8000-00805F9B34FB"
#define LDR_PIN_UUID                "11111111-0007-1000-8000-00805F9B34FB"
#define DISPLAY_ROTATION_UUID       "11111111-0008-1000-8000-00805F9B34FB"
#define DRIVER_UUID                 "11111111-0009-1000-8000-00805F9B34FB"
#define I2C_SPEED_UUID              "11111111-0010-1000-8000-00805F9B34FB"
#define E_PIN_UUID                  "11111111-0011-1000-8000-00805F9B34FB"

// Network Characteristics
#define WIFI_SSID_UUID              "22222222-0001-1000-8000-00805F9B34FB"
#define WIFI_PWD_UUID               "22222222-0002-1000-8000-00805F9B34FB"
#define TIME_ZONE_UUID              "22222222-0003-1000-8000-00805F9B34FB"
#define NTP_SERVER_UUID             "22222222-0004-1000-8000-00805F9B34FB"
#define MANUAL_POSIX_UUID           "22222222-0005-1000-8000-00805F9B34FB"

// Display Service Characteristics
#define CANVAS_FILE_UUID            "33333333-0001-1000-8000-00805F9B34FB"
#define CANVAS_SERVER_UUID          "33333333-0002-1000-8000-00805F9B34FB"

// Device Info Characteristics
#define FIRMWARE_VERSION_UUID       "44444444-0001-1000-8000-00805F9B34FB"
#define FIRMWARE_NAME_UUID          "44444444-0002-1000-8000-00805F9B34FB"
#define CLOCKFACE_NAME_UUID         "44444444-0003-1000-8000-00805F9B34FB"
#define RESTART_UUID                "44444444-0004-1000-8000-00805F9B34FB"

#ifndef CLOCKFACE_NAME
  #define CLOCKFACE_NAME "UNKNOWN"
#endif

// Forward declarations
class CWBLEServer;

// ===== BLE Server Callbacks =====
class CWBLEServerCallbacks: public BLEServerCallbacks {
  public:
    CWBLEServer* bleServer;
    
    void onConnect(BLEServer* pServer) {
      Serial.println("BLE: Client connected");
      StatusController::getInstance()->blink_led(100, 2);
    }
    
    void onDisconnect(BLEServer* pServer) {
      Serial.println("BLE: Client disconnected - restarting advertising");
      delay(500);
      BLEDevice::startAdvertising();
    }
};

// ===== Characteristic Callbacks =====

// Display Brightness
class BrightnessCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      uint8_t value = data[0];
      Serial.printf("BLE: Setting brightness to %d\n", value);
      ClockwiseParams::getInstance()->displayBright = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = ClockwiseParams::getInstance()->displayBright;
    pCharacteristic->setValue(&value, 1);
  }
};

// Auto Brightness Min
class AutoBrightMinCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      uint8_t value = data[0];
      Serial.printf("BLE: Setting auto bright min to %d\n", value);
      ClockwiseParams::getInstance()->autoBrightMin = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = (uint8_t)ClockwiseParams::getInstance()->autoBrightMin;
    pCharacteristic->setValue(&value, 1);
  }
};

// Auto Brightness Max
class AutoBrightMaxCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      uint8_t value = data[0];
      Serial.printf("BLE: Setting auto bright max to %d\n", value);
      ClockwiseParams::getInstance()->autoBrightMax = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = (uint8_t)ClockwiseParams::getInstance()->autoBrightMax;
    pCharacteristic->setValue(&value, 1);
  }
};

// Swap Blue/Green
class SwapBlueGreenCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      bool value = (data[0] == 1);
      Serial.printf("BLE: Setting swap blue/green to %d\n", value);
      ClockwiseParams::getInstance()->swapBlueGreen = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = ClockwiseParams::getInstance()->swapBlueGreen ? 1 : 0;
    pCharacteristic->setValue(&value, 1);
  }
};

// Use 24h Format
class Use24hFormatCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      bool value = (data[0] == 1);
      Serial.printf("BLE: Setting use 24h format to %d\n", value);
      ClockwiseParams::getInstance()->use24hFormat = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = ClockwiseParams::getInstance()->use24hFormat ? 1 : 0;
    pCharacteristic->setValue(&value, 1);
  }
};

// LDR Pin
class LDRPinCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      uint8_t value = data[0];
      Serial.printf("BLE: Setting LDR pin to %d\n", value);
      ClockwiseParams::getInstance()->ldrPin = value;
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = ClockwiseParams::getInstance()->ldrPin;
    pCharacteristic->setValue(&value, 1);
  }
};

// Display Rotation
class DisplayRotationCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1) {
      uint8_t value = data[0];
      if (value <= 3) {
        Serial.printf("BLE: Setting display rotation to %d\n", value);
        ClockwiseParams::getInstance()->displayRotation = value;
        ClockwiseParams::getInstance()->save();
      }
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    uint8_t value = ClockwiseParams::getInstance()->displayRotation;
    pCharacteristic->setValue(&value, 1);
  }
};

// WiFi SSID
class WiFiSsidCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0 && value.length() <= 32) {
      Serial.printf("BLE: Setting WiFi SSID to %s\n", value.c_str());
      ClockwiseParams::getInstance()->wifiSsid = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->wifiSsid.c_str());
  }
};

// WiFi Password
class WiFiPasswordCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0 && value.length() <= 64) {
      Serial.println("BLE: Setting WiFi password");
      ClockwiseParams::getInstance()->wifiPwd = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->wifiPwd.c_str());
  }
};

// Timezone
class TimezoneCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0 && value.length() <= 64) {
      Serial.printf("BLE: Setting timezone to %s\n", value.c_str());
      ClockwiseParams::getInstance()->timeZone = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->timeZone.c_str());
  }
};

// NTP Server
class NTPServerCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0 && value.length() <= 128) {
      Serial.printf("BLE: Setting NTP server to %s\n", value.c_str());
      ClockwiseParams::getInstance()->ntpServer = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->ntpServer.c_str());
  }
};

// Manual POSIX
class ManualPosixCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() <= 64) {
      Serial.printf("BLE: Setting manual POSIX to %s\n", value.c_str());
      ClockwiseParams::getInstance()->manualPosix = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->manualPosix.c_str());
  }
};

// Canvas File
class CanvasFileCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() <= 64) {
      Serial.printf("BLE: Setting canvas file to %s\n", value.c_str());
      ClockwiseParams::getInstance()->canvasFile = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->canvasFile.c_str());
  }
};

// Canvas Server
class CanvasServerCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() <= 128) {
      Serial.printf("BLE: Setting canvas server to %s\n", value.c_str());
      ClockwiseParams::getInstance()->canvasServer = String(value.c_str());
      ClockwiseParams::getInstance()->save();
    }
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    pCharacteristic->setValue(ClockwiseParams::getInstance()->canvasServer.c_str());
  }
};

// Restart Command
class RestartCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    uint8_t* data = pCharacteristic->getData();
    if (pCharacteristic->getValue().length() == 1 && data[0] == 1) {
      Serial.println("BLE: Restart command received - rebooting in 1 second...");
      StatusController::getInstance()->blink_led(100, 5);
      delay(1000);
      ESP.restart();
    }
  }
};

// Placeholder callbacks for not-yet-implemented features
class PlaceholderCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Serial.println("BLE: Write to placeholder characteristic (not implemented)");
  }
  
  void onRead(BLECharacteristic *pCharacteristic) {
    // Return empty or default value
    pCharacteristic->setValue("");
  }
};

// ===== Main BLE Server Class =====
class CWBLEServer {
private:
  BLEServer* pServer;
  CWBLEServerCallbacks* serverCallbacks;
  bool deviceConnected = false;
  bool advertising = false;

public:
  static CWBLEServer* getInstance() {
    static CWBLEServer instance;
    return &instance;
  }

  void begin() {
    Serial.println("BLE: Initializing BLE Server...");
    
    // Initialize BLE
    BLEDevice::init("Clockwise");
    Serial.println("BLE: Device initialized as 'Clockwise'");
    
    // Create BLE Server
    pServer = BLEDevice::createServer();
    serverCallbacks = new CWBLEServerCallbacks();
    serverCallbacks->bleServer = this;
    pServer->setCallbacks(serverCallbacks);
    Serial.println("BLE: Server created");
    
    // Create Services
    BLEService *deviceConfigService = pServer->createService(DEVICE_CONFIG_SERVICE_UUID);
    BLEService *networkService = pServer->createService(NETWORK_SERVICE_UUID);
    BLEService *displayService = pServer->createService(DISPLAY_SERVICE_UUID);
    BLEService *deviceInfoService = pServer->createService(DEVICE_INFO_SERVICE_UUID);
    Serial.println("BLE: Services created");
    
    // Setup Device Config Characteristics
    setupDeviceConfigService(deviceConfigService);
    Serial.println("BLE: Device config characteristics set up");
    
    // Setup Network Characteristics
    setupNetworkService(networkService);
    Serial.println("BLE: Network characteristics set up");
    
    // Setup Display Service Characteristics
    setupDisplayService(displayService);
    Serial.println("BLE: Display characteristics set up");
    
    // Setup Device Info Characteristics
    setupDeviceInfoService(deviceInfoService);
    Serial.println("BLE: Device info characteristics set up");
    
    // Start Services
    deviceConfigService->start();
    networkService->start();
    displayService->start();
    deviceInfoService->start();
    Serial.println("BLE: All services started");
    
    // Start Advertising
    startAdvertising();
    
    Serial.println("BLE: Server initialized and advertising");
    Serial.printf("BLE: Advertising UUID: %s\n", DEVICE_CONFIG_SERVICE_UUID);
    StatusController::getInstance()->blink_led(50, 3);
  }

private:
  void setupDeviceConfigService(BLEService* service) {
    // Display Brightness
    BLECharacteristic *pBrightChar = service->createCharacteristic(
      DISPLAY_BRIGHT_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pBrightChar->setCallbacks(new BrightnessCallbacks());
    uint8_t brightness = ClockwiseParams::getInstance()->displayBright;
    pBrightChar->setValue(&brightness, 1);
    
    // Auto Brightness Min
    BLECharacteristic *pAutoMinChar = service->createCharacteristic(
      AUTO_BRIGHT_MIN_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pAutoMinChar->setCallbacks(new AutoBrightMinCallbacks());
    uint8_t autoMin = (uint8_t)ClockwiseParams::getInstance()->autoBrightMin;
    pAutoMinChar->setValue(&autoMin, 1);
    
    // Auto Brightness Max
    BLECharacteristic *pAutoMaxChar = service->createCharacteristic(
      AUTO_BRIGHT_MAX_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pAutoMaxChar->setCallbacks(new AutoBrightMaxCallbacks());
    uint8_t autoMax = (uint8_t)ClockwiseParams::getInstance()->autoBrightMax;
    pAutoMaxChar->setValue(&autoMax, 1);
    
    // Swap Blue/Green
    BLECharacteristic *pSwapBGChar = service->createCharacteristic(
      SWAP_BLUE_GREEN_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pSwapBGChar->setCallbacks(new SwapBlueGreenCallbacks());
    uint8_t swapBG = ClockwiseParams::getInstance()->swapBlueGreen ? 1 : 0;
    pSwapBGChar->setValue(&swapBG, 1);
    
    // Swap Blue/Red (placeholder)
    BLECharacteristic *pSwapBRChar = service->createCharacteristic(
      SWAP_BLUE_RED_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pSwapBRChar->setCallbacks(new PlaceholderCallbacks());
    uint8_t swapBR = 0;
    pSwapBRChar->setValue(&swapBR, 1);
    
    // Use 24h Format
    BLECharacteristic *p24hChar = service->createCharacteristic(
      USE_24H_FORMAT_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    p24hChar->setCallbacks(new Use24hFormatCallbacks());
    uint8_t use24h = ClockwiseParams::getInstance()->use24hFormat ? 1 : 0;
    p24hChar->setValue(&use24h, 1);
    
    // LDR Pin
    BLECharacteristic *pLDRChar = service->createCharacteristic(
      LDR_PIN_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pLDRChar->setCallbacks(new LDRPinCallbacks());
    uint8_t ldrPin = ClockwiseParams::getInstance()->ldrPin;
    pLDRChar->setValue(&ldrPin, 1);
    
    // Display Rotation
    BLECharacteristic *pRotationChar = service->createCharacteristic(
      DISPLAY_ROTATION_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pRotationChar->setCallbacks(new DisplayRotationCallbacks());
    uint8_t rotation = ClockwiseParams::getInstance()->displayRotation;
    pRotationChar->setValue(&rotation, 1);
    
    // Driver (placeholder)
    BLECharacteristic *pDriverChar = service->createCharacteristic(
      DRIVER_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pDriverChar->setCallbacks(new PlaceholderCallbacks());
    pDriverChar->setValue("HUB75");
    
    // I2C Speed (placeholder)
    BLECharacteristic *pI2CChar = service->createCharacteristic(
      I2C_SPEED_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pI2CChar->setCallbacks(new PlaceholderCallbacks());
    uint32_t i2cSpeed = 400000;
    pI2CChar->setValue((uint8_t*)&i2cSpeed, 4);
    
    // E Pin (placeholder)
    BLECharacteristic *pEPinChar = service->createCharacteristic(
      E_PIN_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pEPinChar->setCallbacks(new PlaceholderCallbacks());
    uint8_t ePin = 18;
    pEPinChar->setValue(&ePin, 1);
  }

  void setupNetworkService(BLEService* service) {
    // WiFi SSID
    BLECharacteristic *pSsidChar = service->createCharacteristic(
      WIFI_SSID_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pSsidChar->setCallbacks(new WiFiSsidCallbacks());
    pSsidChar->setValue(ClockwiseParams::getInstance()->wifiSsid.c_str());
    
    // WiFi Password
    BLECharacteristic *pPwdChar = service->createCharacteristic(
      WIFI_PWD_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pPwdChar->setCallbacks(new WiFiPasswordCallbacks());
    pPwdChar->setValue(ClockwiseParams::getInstance()->wifiPwd.c_str());
    
    // Timezone
    BLECharacteristic *pTzChar = service->createCharacteristic(
      TIME_ZONE_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pTzChar->setCallbacks(new TimezoneCallbacks());
    pTzChar->setValue(ClockwiseParams::getInstance()->timeZone.c_str());
    
    // NTP Server
    BLECharacteristic *pNtpChar = service->createCharacteristic(
      NTP_SERVER_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pNtpChar->setCallbacks(new NTPServerCallbacks());
    pNtpChar->setValue(ClockwiseParams::getInstance()->ntpServer.c_str());
    
    // Manual POSIX
    BLECharacteristic *pPosixChar = service->createCharacteristic(
      MANUAL_POSIX_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pPosixChar->setCallbacks(new ManualPosixCallbacks());
    pPosixChar->setValue(ClockwiseParams::getInstance()->manualPosix.c_str());
  }

  void setupDisplayService(BLEService* service) {
    // Canvas File
    BLECharacteristic *pFileChar = service->createCharacteristic(
      CANVAS_FILE_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pFileChar->setCallbacks(new CanvasFileCallbacks());
    pFileChar->setValue(ClockwiseParams::getInstance()->canvasFile.c_str());
    
    // Canvas Server
    BLECharacteristic *pServerChar = service->createCharacteristic(
      CANVAS_SERVER_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    pServerChar->setCallbacks(new CanvasServerCallbacks());
    pServerChar->setValue(ClockwiseParams::getInstance()->canvasServer.c_str());
  }

  void setupDeviceInfoService(BLEService* service) {
    // Firmware Version (Read-only)
    BLECharacteristic *pVersionChar = service->createCharacteristic(
      FIRMWARE_VERSION_UUID,
      BLECharacteristic::PROPERTY_READ
    );
    pVersionChar->setValue(CW_FW_VERSION);
    
    // Firmware Name (Read-only)
    BLECharacteristic *pNameChar = service->createCharacteristic(
      FIRMWARE_NAME_UUID,
      BLECharacteristic::PROPERTY_READ
    );
    pNameChar->setValue(CW_FW_NAME);
    
    // Clockface Name (Read-only)
    BLECharacteristic *pClockfaceChar = service->createCharacteristic(
      CLOCKFACE_NAME_UUID,
      BLECharacteristic::PROPERTY_READ
    );
    pClockfaceChar->setValue(CLOCKFACE_NAME);
    
    // Restart (Write-only)
    BLECharacteristic *pRestartChar = service->createCharacteristic(
      RESTART_UUID,
      BLECharacteristic::PROPERTY_WRITE
    );
    pRestartChar->setCallbacks(new RestartCallbacks());
  }

  void startAdvertising() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(DEVICE_CONFIG_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
    advertising = true;
    Serial.println("BLE: Advertising started with service UUID");
  }

public:
  void stop() {
    if (advertising) {
      BLEDevice::stopAdvertising();
      advertising = false;
      Serial.println("BLE: Advertising stopped");
    }
  }

  bool isAdvertising() {
    return advertising;
  }
};
