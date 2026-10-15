#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>
#include <EEPROM.h>

#define MAX_GRUPOS 3
#define MAX_FRASES 5
#define MAX_LONGITUD_FRASE 64 // Tamaño fijo seguro para evitar desbordamientos de memoria

class DisplayManager {
  private:
    MD_Parola myDisplay;
    char gruposFrases[MAX_GRUPOS][MAX_FRASES][MAX_LONGITUD_FRASE]; // Almacenamiento seguro en matriz de C-strings
    int grupoSeleccionado;
    int fraseActualIndex;
    char bufferTexto[MAX_LONGITUD_FRASE]; // Búfer activo seguro para la pantalla

  public:
    DisplayManager(MD_MAX72XX::moduleType_t hardwareType, uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices);
    void begin();
    void update();
    
    int getSelectedGroup();
    void setSelectedGroup(int g);
    String getPhrase(int g, int f);
    void setPhrase(int g, int f, String phrase);
    void loadFromEEPROM();
    void saveToEEPROM();
};

#endif