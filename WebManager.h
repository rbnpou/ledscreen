#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include "DisplayManager.h"

class WebManager {
  private:
    ESP8266WebServer server;
    DNSServer dnsServer;
    DisplayManager* displayManager;
    const char* ssid;

    void setupRoutes();

  public:
    WebManager(const char* ssid, DisplayManager* dispMgr);
    void begin();
    void handleClients();
};

#endif