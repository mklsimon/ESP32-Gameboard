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
      padding: 0px;
      border-radius: 5px;
      margin: 0px;
      cursor: pointer;
      width:50px;
    }

    .custom-radio input[type="radio"] {
      display: none;
    }

    .custom-radio input[type="radio"]:checked + .radio-label {
      background-color: green;
      border-radius: 5px;
      border: 2px solid green;
    }
    .channnelBlock{
        margin:0px;
        padding:0px;
        display: flex; 
        align-items: center;
        justify-content: center;   
           
    }
    .rotate-90 {
        transform: rotate(90deg);
        }
  </style>
</head>
<body>
  <table border=0 class="gamepad-table">
    <tr>
        <td></td>
        <td class="arrow arrow-up" onclick="sendControl('up')">↑</td>
        <td colspan="3">
            <div class="channnelBlock">
                <label class="custom-radio"><input type="radio" name="option" value="0"><span class="radio-label">📡 0</span></label>
                <label class="custom-radio"><input type="radio" name="option" value="1"><span class="radio-label">📡 1</span></label>
                <label class="custom-radio"><input type="radio" name="option" value="2"><span class="radio-label">📡 2</span></label>
                <label class="custom-radio"><input type="radio" name="option" value="3"><span class="radio-label">📡 3</span></label>
            </div>
        </td>
        <td class="button button-color" style="background-color: blue" onclick="sendControl('B')">▲</td>
        <td></td>
    </tr>
    <tr>
        <td class="arrow arrow-left" onclick="sendControl('left')">←</td>
        <td></td>  
        <td class="arrow arrow-right" onclick="sendControl('right')">→</td>      
        <td><button class="button button-mini" onClick="toggleFullScreen();" type="button">&#128250;</button></td>
        <td class="button button-color" style="background-color: red;" onclick="sendControl('R')" colspan="3">■</td>
        <td></td>
        <td class="button button-color" style="background-color: yellow" onclick="sendControl('Y')" colspan="3">&#10006;</td>
    </tr>
    <tr>
        <td></td>
        <td class="arrow arrow-down" onclick="sendControl('down')">↓</td>
        <td></td>
        <td><button class="button button-mini" id="rotateButton" onclick="rotateTable()">⟳</button></td>
        <td></td>
        <td class="button button-color" style="background-color: green" onclick="sendControl('G')">&#11044;</td>
        <td></td>
    </tr>
  </table>
</body>
<script>
  const ipAddress = window.location.hostname;
  const webSocketURL = `ws://${ipAddress}/ws`;
  const socket = new WebSocket(webSocketURL);
  function connectWebSocket() {
    socket.onopen = function (event) {
      console.log("WebSocket connection established.");
    };
    socket.onclose = function (event) {
      if (event.code === 1006) {
        // Le code 1006 indique que la connexion a été interrompue de manière anormale.
        console.log("WebSocket connection closed abnormally. Reconnecting...");
        setTimeout(() => {
          connectWebSocket(); // Reconnectez-vous après un court délai
        }, 2000); // Réessayez la connexion après 2 secondes (ajustez ce délai selon vos besoins)
      } else {
        console.log("WebSocket connection closed with code: " + event.code);
      }
    };

    socket.onerror = function (event) {
      console.error("WebSocket error: " + event.message);
    };
  }
  connectWebSocket(); // Démarrez la connexion WebSocket

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

  function rotateTable() {
    const table = document.querySelector(".gamepad-table");
    if (table.classList.contains("rotate-90")) {
        table.classList.remove("rotate-90");
    } else {
        table.classList.add("rotate-90");
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
    //WiFi.mode(WIFI_AP);
    //WiFi.softAP("ESPmatrix");
    // Configurer le serveur DNS asynchrone
    // dnsServer.start(53, "*", WiFi.softAPIP());
    
    const char* ssid = "freebox_GS";     // Remplacez par le nom de votre réseau Wi-Fi
    const char* password = "mickeyfreeboxgs"; 

    // Connexion au Wi-Fi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      // Attendez que la connexion soit établie
      delay(1000);
      Serial.println("Connexion au Wi-Fi en cours...");
    }

    // Une fois connecté, affichez l'adresse IP
    Serial.println("Connecté au Wi-Fi");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());

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
