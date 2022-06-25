#ifndef WiFiConnect_h
#define WiFiConnect_h

#include <WiFi.h>

const char* ssid = "HUAWEI-2.4G-gsuF";
const char* password =  "07627983";

struct WiFiConnect 
{
    void connect() 
    {
        delay(1000);
        WiFi.mode(WIFI_MODE_STA);
        WiFi.disconnect();

        WiFi.begin(ssid, password);
        
        Serial.print("Connecting");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    boolean isConnected() 
    {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif