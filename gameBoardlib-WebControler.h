#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncDNSServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>
#include <map>
AsyncWebSocket ws("/ws");

const char* htmlContent = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      margin: 0;
      background-color: #f0f0f0;
    }

    .gamepad-table {
      border: 2px solid #333;
      border-radius: 10px;
      padding: 10px;
    }

    .button,
    .arrow,
    .button-color,
    .button-channel {
      width: 100px;
      height: 100px;
      border-radius: 50%;
      cursor: pointer;
      transition: transform 0.1s, background-color 0.1s;
      box-shadow: 3px 3px 5px #888888;
      display: flex;
      justify-content: center;
      align-items: center;
      font-size: 24px;
    }
    .button-mini{
        width: 50px;
        height: 50px;
    }

    .arrow-up,
    .arrow-down,
    .arrow-left,
    .arrow-right,
    .button-color {
      background-color: #007bff;
    }

    .button-color[style='background-color: red'] {
      background-color: red;
    }

    .button-color[style='background-color: green'] {
      background-color: green;
    }

    .button-color[style='background-color: blue'] {
      background-color: blue;
    }

    .button-color[style='background-color: yellow'] {
      background-color: yellow;
    }

    .button:hover {
      transform: scale(1.05);
    }
    .button:active {
      transform: scale(0.95);
      background-color: #666;
    }

    td.rotate {
      transform: rotate(90deg);
    }

    .custom-radio {
      display: inline-block;
      background-color: #eee;
      border: 1px solid #ccc;
      padding: 10px;
      border-radius: 5px;
      margin: 5px;
      cursor: pointer;
    }

    .custom-radio input[type="radio"] {
      display: none;
    }

    .custom-radio input[type="radio"]:checked + .radio-label {
      background-color: green;
      border-radius: 5px;
      border: 2px solid green;
    }
  </style>
</head>
<body>
  <table class="gamepad-table">
    <tr>
      <td></td>
      <td class="arrow arrow-left" onclick="sendControl('left')">↑</td>
      <td></td>
      <td></td>
    </tr>
    <tr>
      <td class="arrow arrow-down" onclick="sendControl('down')">←</td>
      <td></td>
      <td class="arrow arrow-up" onclick="sendControl('up')">→</td>
      <td></td>
    </tr>
    <tr>
      <td></td>
      <td class="arrow arrow-right" onclick="sendControl('right')">↓</td>
      <td></td>
      <td class="rotate">
        <label class="custom-radio">
          <input type="radio" name="option" value="0">
          <span class="radio-label">📡 0</span>
        </label>
      </td>
    </tr>
    <tr>
      <td></td>
      <td></td>
      <td>
        <button class="button button-mini" onClick="toggleFullScreen();" type="button">&#128250;</button>
      </td>
      <td class="rotate">
        <label class="custom-radio">
          <input type="radio" name="option" value="1">
          <span class="radio-label">📡 1</span>
        </label>
      </td>
    </tr>
    <tr>
      <td></td>
      <td class="button button-color" style="background-color: red;" onclick="sendControl('R')" colspan="3">■</td>
      <td></td>
      <td class="rotate">
        <label class="custom-radio">
          <input type="radio" name="option" value="2">
          <span class="radio-label">📡 2</span>
        </label>
      </td>
    </tr>
    <tr>
      <td class="button button-color" style="background-color: green" onclick="sendControl('G')">&#11044;</td>
      <td></td>
      <td class="button button-color" style="background-color: blue" onclick="sendControl('B')">▲</td>
      <td></td>
    </tr>
    <tr>
      <td></td>
      <td class="button button-color" style="background-color: yellow" onclick="sendControl('Y')" colspan="3">&#10006;</td>
      <td></td>
      <td></td>
    </tr>
  </table>
</body>
<script>
  const ipAddress = window.location.hostname;
  const webSocketURL = `ws://${ipAddress}/ws`;
  const socket = new WebSocket(webSocketURL);

  socket.onopen = function (event) {
    console.log("WebSocket connection established.");
  };

  function sendControl(control) {
    const radioButtons = document.getElementsByName("option");
    for (let i = 0; i < radioButtons.length; i++) {
        if (radioButtons[i].checked) {
            const selectedValue = radioButtons[i].value;
            const data = {
                player: selectedValue,
                move: control
                };
            console.log(data);
            socket.send(JSON.stringify(data));
        }
    }
  }

  function toggleFullScreen() {
    if (!document.fullscreenElement && !document.mozFullScreenElement && !document.webkitFullscreenElement) {
      const docEl = document.documentElement;
      const requestFullScreen = docEl.requestFullscreen || docEl.mozRequestFullScreen || docEl.webkitRequestFullScreen;
      requestFullScreen.call(docEl);
    } else {
      const cancelFullScreen = document.exitFullscreen || document.mozCancelFullScreen || document.webkitExitFullscreen;
      cancelFullScreen.call(document);
    }
  }
</script>
</html>
)rawliteral";


class WebControler {
public:
  WebControler(int port = 80) : webServer(port) {}

  void begin() {
    // Configurer le point d'accès WiFi pour le portail captif
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESPmatrix");
    
    // Configurer le serveur DNS asynchrone
    dnsServer.start(53, "*", WiFi.softAPIP());

    // WebServer
    webServer.on("/", HTTP_GET, std::bind(&WebControler::handleRoot, this, std::placeholders::_1));
    webServer.onNotFound(std::bind(&WebControler::handleRoot, this, std::placeholders::_1));
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
      String message = String((char*)data);
      Serial.println("Message received: " + message);
      handleMessage(message);
    });
    webServer.addHandler(&ws); 
    webServer.begin();
  }

  void handleMessage(const String& jsonMessage) {
    DynamicJsonDocument doc(1024); // Créer un document JSON avec une taille maximale de 1024 octets
    DeserializationError error = deserializeJson(doc, jsonMessage);

    if (error) {
      Serial.print("Erreur lors de l'analyse JSON: ");
      Serial.println(error.c_str());
      return; // Quitter la fonction en cas d'erreur
    }

    // Si l'analyse JSON est réussie, vous pouvez extraire les données et les stocker dans un dictionnaire (map)
    String key = doc["player"].as<String>();  // Remplacez "votre_clef" par le nom de la clé JSON que vous voulez utiliser comme clé dans le dictionnaire
    String value = doc["move"].as<String>(); // Remplacez "votre_champ" par le nom du champ JSON que vous voulez extraire
    dataMap[key]=value;
  }
 
  void handleRoot(AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlContent);
  }

  bool gotMessages(int id) {
    String key = String(id);
    return !dataMap[key].isEmpty();
  }

  String getMessages(int id) {
    String key = String(id);
    String message = dataMap[key]; // Stocker la valeur associée à la clé
    dataMap[key].clear(); // Vider la valeur associée à la clé
    return message; // Retourner la valeur précédemment associée à la clé
  }
private:
  AsyncDNSServer dnsServer;
  AsyncWebServer webServer;
  std::map<String, String> dataMap;
  static WebControler* instance; 
};
