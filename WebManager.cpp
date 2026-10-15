#include "WebManager.h"

// HTML mejorado con manejo de errores y depuración visual
const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Configuración de ledscreen</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; background: #f4f7f6; color: #333; padding: 15px; max-width: 480px; margin: auto; }
        .card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 4px 10px rgba(0,0,0,0.08); margin-bottom: 20px; }
        h2 { color: #007bff; text-align: center; margin-top: 0; }
        h3 { border-bottom: 2px solid #007bff; padding-bottom: 5px; color: #444; }
        button { background: #007bff; color: white; border: none; padding: 12px 15px; border-radius: 8px; cursor: pointer; font-size: 15px; font-weight: bold; width: 100%; transition: background 0.2s; margin-top: 5px;}
        button:hover { background: #0056b3; }
        .btn-show { background: #28a745; width: auto; padding: 6px 12px; font-size: 13px; float: right; border-radius: 6px; }
        .btn-show:hover { background: #218838; }
        input[type="text"] { width: 100%; padding: 10px; margin: 6px 0 12px 0; border: 1px solid #ccc; border-radius: 6px; font-size: 15px; box-sizing: border-box; }
        .group-box { background: #fafbfc; border: 1px solid #e1e4e8; padding: 12px; border-radius: 8px; margin-bottom: 15px; }
        .active-badge { color: #28a745; font-size: 13px; font-weight: bold; }
    </style>
</head>
<body>
    <div class="card">
        <h2>Led Screen</h2>
        <p style="text-align:center; color:#666; font-size:14px;">Configura tus 3 grupos de frases.</p>
        <div id="editor">Conectando y cargando datos...</div>
        <button onclick="guardarTodo()" style="background:#28a745; margin-top: 15px;">💾 Guardar todos los cambios</button>
        <p style="text-align:center; color:#666; font-size:14px;">LED screen by @rbnpou</p>
    </div>

<script>
    let datosGlobales = [];
    let grupoActivo = 0;

    async function cargarDatos() {
        try {
            let res = await fetch('/getdata');
            if (!res.ok) throw new Error("Error en servidor");
            let data = await res.json();
            datosGlobales = data.grupos;
            grupoActivo = data.activo;
            renderizar();
        } catch (e) {
            document.getElementById('editor').innerHTML = '<p style="color:red; text-align:center;">Error cargando datos. Toca aquí para <button onclick="cargarDatos()">Reintentar</button></p>';
        }
    }

    function renderizar() {
        let html = "";
        for(let g=0; g<3; g++) {
            let esActivo = (g === grupoActivo);
            let estadoTexto = esActivo ? `<span class="active-badge">🟢 [ACTIVO]</span>` : "";
            
            html += `<div class="group-box">
                        <h3>Grupo ${g+1} ${estadoTexto} <button class="btn-show" onclick="seleccionar(${g})">Mostrar</button></h3>`;
            
            for(let f=0; f<5; f++) {
                // Validación estricta para evitar nulos
                let val = (datosGlobales[g] && datosGlobales[g][f]) ? datosGlobales[g][f] : "";
                html += `<label style="font-size:13px; color:#555;">Frase ${f+1}:</label>
                         <input type="text" id="g${g}_f${f}" value="${val}" placeholder="Escribe tu frase...">`;
            }
            html += `</div>`;
        }
        document.getElementById('editor').innerHTML = html;
    }

    async function seleccionar(g) {
        try {
            await fetch(`/select?g=${g}`);
            alert(`¡Grupo ${g+1} activado en la matriz!`);
            cargarDatos();
        } catch(e) {
            alert("Error al cambiar de grupo.");
        }
    }

    async function guardarTodo() {
        let nuevosGrupos = [];
        for(let g=0; g<3; g++) {
            let frases = [];
            for(let f=0; f<5; f++) {
                let input = document.getElementById(`g${g}_f${f}`);
                frases.push(input ? input.value : "");
            }
            nuevosGrupos.push(frases);
        }
        
        try {
            let res = await fetch('/save', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ grupos: nuevosGrupos })
            });
            
            if(res.ok) {
                alert("¡Cambios guardados con éxito en la memoria!");
            } else {
                alert("Error al guardar en el dispositivo.");
            }
        } catch(e) {
            alert("Error de conexión al guardar.");
        }
        cargarDatos();
    }

    // Iniciar carga al abrir la página
    cargarDatos();
</script>
</body>
</html>
)rawliteral";

WebManager::WebManager(const char* ssid, DisplayManager* dispMgr) : server(80), ssid(ssid), displayManager(dispMgr) {}

void WebManager::begin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  // Potencia Wi-Fi al mínimo
  WiFi.setOutputPower(0.0);

  dnsServer.start(53, "*", WiFi.softAPIP());
  setupRoutes();
  server.begin();
}

void WebManager::handleClients() {
  dnsServer.processNextRequest();
  server.handleClient();
}

void WebManager::setupRoutes() {
  server.on("/", HTTP_GET, [this]() {
    server.send_P(200, "text/html", paginaHTML);
  });

  server.on("/generate_204", HTTP_GET, [this]() { 
    server.sendHeader("Location", "http://192.168.4.1/", true); 
    server.send(302, "text/plain", ""); 
  });
  
  server.on("/fwlink", HTTP_GET, [this]() { 
    server.sendHeader("Location", "http://192.168.4.1/", true); 
    server.send(302, "text/plain", ""); 
  });

  // Ruta para enviar los datos JSON al navegador
  server.on("/getdata", HTTP_GET, [this]() {
    StaticJsonDocument<1500> doc;
    doc["activo"] = displayManager->getSelectedGroup();
    JsonArray arr = doc.createNestedArray("grupos");
    
    for (int g = 0; g < MAX_GRUPOS; g++) {
      JsonArray subArr = arr.createNestedArray();
      for (int f = 0; f < MAX_FRASES; f++) {
        subArr.add(displayManager->getPhrase(g, f));
      }
    }
    String jsonStr;
    serializeJson(doc, jsonStr);
    server.send(200, "application/json", jsonStr);
  });

  server.on("/select", HTTP_GET, [this]() {
    if (server.hasArg("g")) {
      int g = server.arg("g").toInt();
      displayManager->setSelectedGroup(g);
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/save", HTTP_POST, [this]() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<1500> doc;
      DeserializationError error = deserializeJson(doc, server.arg("plain"));
      if (!error) {
        JsonArray arr = doc["grupos"];
        for (int g = 0; g < MAX_GRUPOS; g++) {
          for (int f = 0; f < MAX_FRASES; f++) {
            String textoFrase = arr[g][f].as<String>();
            displayManager->setPhrase(g, f, textoFrase);
          }
        }
        displayManager->saveToEEPROM();
        server.send(200, "application/json", "{\"status\":\"ok\"}");
        return;
      }
    }
    server.send(400, "application/json", "{\"status\":\"error\"}");
  });
}