#pragma once

#include <WiFi.h>
#include <WiFiClientSecure.h>

struct ClockwiseHttpClient
{

  static ClockwiseHttpClient *getInstance()
  {
    static ClockwiseHttpClient base;
    return &base;
  }

  void httpGet(WiFiClientSecure *client, const char *host, const char *path, const uint16_t port)
  {
    Serial.printf("[HTTP] GET request to '%s%s' on port %d\n", host, path, port);

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Not connected");
      return;
    }

    client->setInsecure();
    client->setTimeout(10000);
    if (!client->connect(host, port))
    {
      Serial.println(F("Connection failed"));
      return;
    }

    // Send HTTP request
    client->printf("GET %s HTTP/1.1\r\n", path);
    client->printf("Host: %s\r\n", host);
    client->println(F("Connection: close"));
    if (client->println() == 0)
    {
      Serial.println(F("Failed to send request"));
      client->stop();
      return;
    }

    // char arrCode[4];
    // memcpy(arrCode, status + 8, 3);  //HTTP/1.1 404 Not Found
    // arrCode[3] = 0;
    // uint16_t httpCode = atoi(arrCode);

    // Check HTTP status
    char status[32] = {0};
    client->readBytesUntil('\r', status, sizeof(status));

    if (strstr(status, "200 OK") == NULL)
    {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      client->stop();
      return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client->find(endOfHeaders))
    {
      Serial.println(F("Invalid response"));
      client->stop();
      return;
    }
  }
};
