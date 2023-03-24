#pragma once

#include "ImprovWiFiLibrary.h"
#include <WiFi.h>
#include <CWPreferences.h>
#include "IOManager.h"


WiFiServer server(80);
ImprovWiFi improvSerial(&Serial);

struct ClockwiseWebServer
{

    char linebuf[80];
    int charcount = 0;
    

    static void onImprovWiFiErrorCb(improv::Error err)
    {
        server.stop();
        IOManager::blink_led(2000, 3);
    }

    static void onImprovWiFiConnectedCb(std::string ssid, std::string password)
    {
        ClockwiseParams::getInstance()->load();
        ClockwiseParams::getInstance()->wifiSsid = String(ssid.c_str());
        ClockwiseParams::getInstance()->wifiPwd = String(password.c_str());
        ClockwiseParams::getInstance()->save();

        startCWWebServer();
    }

    static void startCWWebServer() {
        server.begin();
        IOManager::blink_led(100, 3);
    }

    void begin()
    {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();

        improvSerial.setDeviceInfo(improv::ChipFamily::CF_ESP32, "CW-20230324", "1.1.0", "Clockwise");
        improvSerial.onImprovWiFiError(onImprovWiFiErrorCb);
        improvSerial.onImprovWiFiConnected(onImprovWiFiConnectedCb);
        
        ClockwiseParams::getInstance()->load();
        if (improvSerial.tryConnectToWifi(ClockwiseParams::getInstance()->wifiSsid.c_str(), ClockwiseParams::getInstance()->wifiPwd.c_str())) {
            startCWWebServer();
        }

    }

    void handleHttpRequest()
    {

        improvSerial.handleSerial();

        if (!improvSerial.isConnected())
            return;

        WiFiClient client = server.available();

        if (client)
        {
            IOManager::blink_led(100, 1);
            memset(linebuf, 0, sizeof(linebuf));
            charcount = 0;
            // an http request ends with a blank line
            boolean currentLineIsBlank = true;
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
                    // read char by char HTTP request
                    linebuf[charcount] = c;
                    if (charcount < sizeof(linebuf) - 1)
                        charcount++;

                    if (c == '\n' && currentLineIsBlank)
                    {
                        // send a standard http response header
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close"); // the connection will be closed after completion of the response
                        client.println();
                        client.println("<!DOCTYPE HTML><html><body>");
                        client.println("<h1 id=\"welcome\">Welcome!</h1>");
                        client.println("<p>This is a simple webpage served by your ESP32</p>");
                        client.println("</body></html>");
                        break;
                    }
                }
            }
            delay(1);
            client.stop();
        }
    }

    
};
