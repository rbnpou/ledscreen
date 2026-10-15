#include "DisplayManager.h"

DisplayManager::DisplayManager(MD_MAX72XX::moduleType_t hardwareType, uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t numDevices)
  : myDisplay(hardwareType, dataPin, clkPin, csPin, numDevices) {
  grupoSeleccionado = 0;
  fraseActualIndex = 0;
}

void DisplayManager::begin() {
  EEPROM.begin(512); 
  myDisplay.begin();
  myDisplay.setIntensity(5);
  myDisplay.displayClear();
  
  loadFromEEPROM(); // Carga el grupo activo y todas las frases guardadas
  
  strncpy(bufferTexto, gruposFrases[grupoSeleccionado][fraseActualIndex], MAX_LONGITUD_FRASE);
  if (strlen(bufferTexto) == 0) {
    strcpy(bufferTexto, "LED screen by @rbnpou");
  }
  
  myDisplay.displayText(bufferTexto, PA_CENTER, 90, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void DisplayManager::update() {
  if (myDisplay.displayAnimate()) {
    int intentos = 0;
    do {
      fraseActualIndex = (fraseActualIndex + 1) % MAX_FRASES;
      intentos++;
      if (strlen(gruposFrases[grupoSeleccionado][fraseActualIndex]) > 0) {
        break;
      }
    } while (intentos < MAX_FRASES);

    strncpy(bufferTexto, gruposFrases[grupoSeleccionado][fraseActualIndex], MAX_LONGITUD_FRASE);
    
    if (strlen(bufferTexto) == 0) {
      strcpy(bufferTexto, "Grupo Vacio");
    }
    
    myDisplay.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    myDisplay.displayText(bufferTexto, PA_CENTER, 90, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    myDisplay.displayReset();
  }
}

int DisplayManager::getSelectedGroup() {
  return grupoSeleccionado;
}

void DisplayManager::setSelectedGroup(int g) {
  if (g >= 0 && g < MAX_GRUPOS) {
    grupoSeleccionado = g;
    fraseActualIndex = 0;
    myDisplay.displayClear();
    
    strncpy(bufferTexto, gruposFrases[grupoSeleccionado][fraseActualIndex], MAX_LONGITUD_FRASE);
    if (strlen(bufferTexto) == 0) {
      snprintf(bufferTexto, MAX_LONGITUD_FRASE, "Grupo %d", g + 1);
    }
    
    myDisplay.displayText(bufferTexto, PA_CENTER, 90, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    myDisplay.displayReset();
    
    // Guardar el grupo activo en la EEPROM (Byte 0)
    EEPROM.write(0, grupoSeleccionado);
    EEPROM.commit();
  }
}

String DisplayManager::getPhrase(int g, int f) {
  if (g >= 0 && g < MAX_GRUPOS && f >= 0 && f < MAX_FRASES) {
    return String(gruposFrases[g][f]);
  }
  return "";
}

void DisplayManager::setPhrase(int g, int f, String phrase) {
  if (g >= 0 && g < MAX_GRUPOS && f >= 0 && f < MAX_FRASES) {
    phrase.trim();
    strncpy(gruposFrases[g][f], phrase.c_str(), MAX_LONGITUD_FRASE);
    gruposFrases[g][f][MAX_LONGITUD_FRASE - 1] = '\0';
  }
}

void DisplayManager::loadFromEEPROM() {
  // 1. Leer grupo activo del Byte 0
  int storedGroup = EEPROM.read(0);
  if (storedGroup >= 0 && storedGroup < MAX_GRUPOS) {
    grupoSeleccionado = storedGroup;
  } else {
    grupoSeleccionado = 0;
  }

  // 2. Leer frases con bloques de tamaño fijo (MAX_LONGITUD_FRASE bytes por cada frase)
  int address = 1;
  for (int g = 0; g < MAX_GRUPOS; g++) {
    for (int f = 0; f < MAX_FRASES; f++) {
      for (int i = 0; i < MAX_LONGITUD_FRASE; i++) {
        gruposFrases[g][f][i] = EEPROM.read(address++);
      }
      // Asegurar el fin de cadena por seguridad
      gruposFrases[g][f][MAX_LONGITUD_FRASE - 1] = '\0';
    }
  }

  // Si la EEPROM está virgen (primer caracter nulo o basura), inicializamos valores por defecto
  if (gruposFrases[0][0][0] == '\0' || (uint8_t)gruposFrases[0][0][0] == 255) {
    for (int g = 0; g < MAX_GRUPOS; g++) {
      for (int f = 0; f < MAX_FRASES; f++) {
        gruposFrases[g][f][0] = '\0';
      }
    }
    strcpy(gruposFrases[0][0], "LED screen by @rbnpou");
    strcpy(gruposFrases[0][1], "Matriz 32x8 OK");
    saveToEEPROM();
  }
}

void DisplayManager::saveToEEPROM() {
  // 1. Guardar grupo activo en el Byte 0
  EEPROM.write(0, grupoSeleccionado);

  // 2. Escribir cada frase asegurando bloques de tamaño fijo (MAX_LONGITUD_FRASE bytes exactos)
  int address = 1;
  for (int g = 0; g < MAX_GRUPOS; g++) {
    for (int f = 0; f < MAX_FRASES; f++) {
      for (int i = 0; i < MAX_LONGITUD_FRASE; i++) {
        EEPROM.write(address++, gruposFrases[g][f][i]);
      }
    }
  }
  
  EEPROM.commit(); // Volcar cambios permanentemente a la flash del ESP8266
}