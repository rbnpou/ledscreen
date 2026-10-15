#include "DisplayManager.h"
#include "WebManager.h"

// Configuración de pines para ESP8266 (NodeMCU / WeMos D1 Mini)
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES   4    // Número de módulos en cascada
#define DATA_PIN      13   // D7 (MOSI)
#define CLK_PIN       14   // D5 (SCK)
#define CS_PIN        15   // D8 (CS / LOAD)

const char* ssidAP = "Led Screen by @rbnpou";

DisplayManager displayMgr(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
WebManager webMgr(ssidAP, &displayMgr);

void setup() {
  Serial.begin(115200);

  displayMgr.begin();
  webMgr.begin();

  Serial.println("Sistema ledscreen iniciado correctamente.");
}

void loop() {
  webMgr.handleClients();
  displayMgr.update();
}