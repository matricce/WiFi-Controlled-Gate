#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <FS.h>
#else
#include <WiFi.h>
#include <SPIFFS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "credentials.h"

#define GATE_PIN 5
#define LED_PIN 2
#define CONTROL_TIME_ACTIVADED 1000 //tempo em que o controle se mantém acionado
#define MAX_CLIENTS (WEBSOCKETS_SERVER_CLIENT_MAX + 1)

//Descomentar e inserir os dados do wifi para o esp se conectar
//const char* ssid = "";
//const char* password = "";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);

// IPAddress staticIP(192, 168, 0, 2);
// IPAddress gateway(192, 168, 0, 1);
// IPAddress subnet(255, 255, 255, 0);

String controlSwitch;
bool controlState = false;
String JSONtxt;
uint8_t qtdClients;
uint16_t ledBlinkDelay = 1000;

typedef struct connections {
  IPAddress ip;
  bool state;
  uint32_t sinceLastConnection;
  bool updateMe;
  uint32_t sinceLastMessage;
}IPs;

IPs clients[WEBSOCKETS_SERVER_CLIENT_MAX + 1];

char *timeOn();
void setupWiFi();
void setupSPIFFS();
void setupWebSocket();
void setupServer();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght);
void onIndexRequest(AsyncWebServerRequest * request);
void onJavascriptRequest(AsyncWebServerRequest * request);
void onCSSRequest(AsyncWebServerRequest * request);
void onPageNotFound(AsyncWebServerRequest * request);
void verifyGateStatus();
void verifyClients();
void updateClients();
void disconnectClients();
String formatBytes(size_t bytes);
void ledBlink();

void setup() {
  Serial.begin(115200);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  setupWiFi();
  setupSPIFFS();
  setupWebSocket();
  setupServer();
}

void loop() {
  webSocket.loop();
  verifyGateStatus();
  verifyClients();
  updateClients();
  disconnectClients();
  ledBlink();
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print('.');
  }
}

void setupSPIFFS() {
  if (!SPIFFS.begin()) {
    while (1);
  }
  Serial.print("SPIFFS started. Contents:\n");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
  }
  Serial.print("\n");
}

void setupWebSocket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.print("WebSocket server started.\n");
}

void setupServer() {
  server.on("/", HTTP_GET, onIndexRequest);
  server.on("/script.js", HTTP_GET, onJavascriptRequest);
  server.on("/styles.css", HTTP_GET, onCSSRequest);
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();
  Serial.print("HTTP server started.\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght) {
  IPAddress ip = webSocket.remoteIP(num);
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    clients[num].state = 0;
    clients[num].sinceLastMessage = 0;
    break;
  case WStype_CONNECTED:
    Serial.printf("[%d] Connected with IP: %d.%d.%d.%d", num, ip[0], ip[1], ip[2], ip[3]);
    clients[num].state = 1;
    clients[num].ip = ip;
    clients[num].sinceLastConnection = millis();
    clients[num].sinceLastMessage = millis();
    break;
  case WStype_TEXT:
    clients[num].state = 1;
    clients[num].ip = ip;
    clients[num].updateMe = 1;
    clients[num].sinceLastMessage = millis();
    if (strcmp((char * ) payload, "ping") != 0) {
      String payloadString = (const char * ) payload;
      byte separator = payloadString.indexOf('=');
      String var = payloadString.substring(0, separator);
      String val = payloadString.substring(separator + 1);
      if (var == "setControl") { //o app só pode ativar o controle
        controlState = true;
      }
    }
    break;
  default:
    break;
  }
}

void onIndexRequest(AsyncWebServerRequest * request) {
  IPAddress remote_ip = request -> client() -> remoteIP();
  Serial.println("[" + remote_ip.toString() +
    "] HTTP GET request of " + request -> url());
  request -> send(SPIFFS, "/index.html", "text/html");
}

void onJavascriptRequest(AsyncWebServerRequest * request) {
  IPAddress remote_ip = request -> client() -> remoteIP();
  Serial.println("[" + remote_ip.toString() +
    "] HTTP GET request of " + request -> url());
  request -> send(SPIFFS, "/script.js", "text/javascript");
}

void onCSSRequest(AsyncWebServerRequest * request) {
  IPAddress remote_ip = request -> client() -> remoteIP();
  Serial.println("[" + remote_ip.toString() +
"] HTTP GET request of " + request -> url());
  request -> send(SPIFFS, "/styles.css", "text/css");
}

void onPageNotFound(AsyncWebServerRequest * request) {
  IPAddress remote_ip = request -> client() -> remoteIP();
  Serial.println("[" + remote_ip.toString() +
    "] HTTP GET request of " + request -> url());
  request -> send(404, "text/plain", "Not found");
}

char *timeOn() {
  uint32_t duracao = millis();
  uint8_t segundos = ((duracao / 1000) % 60);
  uint8_t minutos = ((duracao / (1000 * 60)) % 60);
  uint8_t horas = ((duracao / (1000 * 60 * 60)) % 24);
  uint8_t dias = ((duracao / (1000 * 60 * 60 * 24)));
  char *tempo = "99d 23h 59m 59s";
  sprintf(tempo, "%02id %02ih %02im %02is", dias, horas, minutos, segundos);
  return tempo;
}

void verifyGateStatus() {
  static uint32_t controlTimer;
  if (!controlState) {
    controlTimer = millis();
  }
  if (millis() - controlTimer > CONTROL_TIME_ACTIVADED) {
    controlState = false;
  }
  digitalWrite(GATE_PIN, controlState);
  controlSwitch = "OFF";
  if (controlState) {
    controlSwitch = "ON";
  }
}

void verifyClients() {
  qtdClients = 0;
  for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
    if(clients[i].state)
      qtdClients++;
    
  }
}

void updateClients() {
  String JSONtxt = "{\"controlState\":\"" + controlSwitch + "\", \"timeOn\":\"" + timeOn() + "\", \"qtdClients\":\"" + qtdClients + "\"}";
  for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
    if (clients[i].state) {
      if (clients[i].updateMe) {
        webSocket.sendTXT(i, JSONtxt);
        clients[i].updateMe = 0;
      }
    }
  }
}
void disconnectClients() {
    ledBlinkDelay = 250;
  if (!qtdClients) {
    ledBlinkDelay = 1000;
    return;
  }
  static uint32_t wait1000ms;
  if (millis() - wait1000ms > 1000) {
    for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].state) {
        if (millis() - clients[i].sinceLastMessage > 10000) {
          Serial.printf("Cliente [%u] ocioso, sendo desconectado!\n", i);
          webSocket.disconnect(i);
          clients[i].state = 0;
        }
      }
      if (clients[i].state) {
        Serial.printf("Cliente: %d // Estado: %d // Update %d // IP: %d.%d.%d.%d // Tempo: %lus // Ocioso: %lus\n",
          i,
          clients[i].state,
          clients[i].updateMe,
          clients[i].ip[0], clients[i].ip[1], clients[i].ip[2], clients[i].ip[3],
          (clients[i].state) ? (millis() - clients[i].sinceLastConnection) / 1000 : 0,
          (clients[i].state) ? (millis() - clients[i].sinceLastMessage) / 1000 : 0);
          Serial.printf("Free HEAP: %d bytes\n", ESP.getFreeHeap());
      }
    }
    wait1000ms = millis();
  }
}
String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

void ledBlink() {
  static unsigned long delayLed = 0;
  if (millis() - delayLed > ledBlinkDelay) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delayLed = millis();
  }
}