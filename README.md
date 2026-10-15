# 🛑 LED screen | Frases a donde vayas

Un sistema de matriz LED modular y completamente **offline** basado en un ESP8266 y módulos MAX7219 (configuración de 4 módulos en cascada, resolución de 32x8). 

Diseñado bajo la premisa de que **un letrero de texto no debería requerir un registro de usuario, un waiver de privacidad, una suscripción mensual, un framework de 2GB de Node.js ni una aplicación móvil con anuncios** solo para mostrar un mensajes en tu escritorio o llavero.

## Breve historia

Lo vi durante la Eko Party 2026 y pensé "que buena idea". Y le sumé algunas características. Pensado originalmente como llavero o letrero portátil para una mochila o bandolera (inclusive para tener sobre el escritorio). Quizás en un futuro podría conectarlo a internet, visualizar el pronóstico o sabér cuando caen los mercados...

---

## ✨ Características Principales

* **100% Offline y Autónomo:** Funciona sin internet, sin routers externos y sin depender de servidores de terceros.
* **Portal Cautivo Automático:** Al encenderse, crea su propia red Wi-Fi (`ledscreen`). Tu celular o portátil se conecta y abre la configuración mediante un servidor DNS propio.
* **Seguridad por Proximidad Física:** Limité la potencia del wifi (`WiFi.setOutputPower(0.0)`) para forzar que **tengas que estar físicamente cerca** si querés cambiar los textos. Si alguien quiere hackear el equipo, tendrá que estar pegado (literalmente). Y si alguien te "hackea"... bienvenida la experiencia!
* **Persistencia en EEPROM:** Las frases sobreviven a cortes de luz, reinicios accidentales y al pánico existencial del microcontrolador.
* **Gestión por Grupos:** Organiza tus mensajes en **3 grupos independientes de 5 frases cada uno**, con selección manual instantánea desde la web.

---

## 🛠️ Hardware Requerido

* **Microcontrolador:** ESP8266 (NodeMCU o WeMos D1 Mini). Barato, consume poco y tiene Wi-Fi integrado.
* **Pantalla:** Módulo LED Dot Matrix MAX7219 (Pack de 4 en 1 integrado, resolución total de 32x8 píxeles).
* **Conexión SPI estándar:**
  * **DIN (Data):** Pin D7 / GPIO 13
  * **CLK (Clock):** Pin D5 / GPIO 14
  * **CS / LOAD:** Pin D8 / GPIO 15

---

## 📂 Estructura Modular del Proyecto

El código está estructurado de manera limpia para evitar el típico espagueti de un solo archivo `.ino` de 2000 líneas que nadie se atreve a tocar:

```text
ESP8266_Matrix_Controller/
│
├── ledscreen.ino                   # Archivo principal (Setup y Loop)
├── DisplayManager.h                # Control de hardware y EEPROM
├── DisplayManager.cpp              # Lógica de renderizado, buffers y persistencia
├── WebManager.h                    # Servidor web y portal cautivo
└── WebManager.cpp                  # Rutas HTTP, DNS y HTML embebido (Portal Cautivo)
```
---

## ⚙️ Detalles Técnicos (O por qué este código no se crashea cada 5 minutos)

Si alguna vez usaste la librería **MD_Parola** con objetos String dinámicos de Arduino en un ESP8266, probablemente sufriste reinicios por culpa del Watchdog debido a la fragmentación de la memoria RAM y punteros colgantes.

Para solucionar esto de raíz, este proyecto implementé:

C-Strings Estáticos (char[]): Se evita el uso de String en los búferes críticos de animación de la matriz. Cada frase cuenta con un bloque estricto y reservado de 64 bytes en memoria RAM y EEPROM (MAX_LONGITUD_FRASE).

Control de Bucles Vacíos: Se implementaron validaciones de longitud mediante strlen() y contadores de intentos seguros para saltar casillas vacías y que el microcontrolador no caiga en un bucle infinito.

## 🚀 Instalación y Uso
Cloná este repositorio o descargá los archivos en tu carpeta de proyectos.
Asegurate de tener instaladas las siguientes librerías desde el gestor de Arduino:

  * **MD_Parola (de MajicDesigns)**
  * **MD_MAX72XX (de MajicDesigns)**

ArduinoJson (para manejar el intercambio de datos con la interfaz web)

Abrí ESP8266_Matrix_Controller.ino en el IDE de Arduino.

Seleccioná la placa ESP8266 y compilá/subí el código.

Configuración:

Busca la red Wi-Fi **lescreen** desde el celular o computadora.

Conectate y se abrirá el portal cautivo automáticamente. O ingresá a http://192.168.4.1 en un navegador.

Escribí tus frases, seleccioná el grupo que querés activar, y pulsá GUARDAR. ¡Listo!

## 💡 Futuras Mejoras (Pull Requests bienvenidos)

¿Control de brillo desde la web? Posible.

¿Integrar un sensor de luz ambiental para no quedar ciego de noche? Tal vez.

¿Giroscopio para detectar la orientación y poder visualizar frases en vertical? Probablemente.

¿Conectarlo a una inteligencia artificial para que el esp8266 reflexione sobre el sentido de la vida? Naa.

Mantengamos las cosas simples y locales.

... por ahora o hasta que me aburra.

Gracias.

**@rbnpou**
