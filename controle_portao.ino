#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include "index.h"
#include "credentials.h"

#define GATE_PIN 5

//const char* ssid = "";
//const char* password = "";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(88);

IPAddress staticIP(192, 168, 15, 200); //200 'testes' // 201 'oficial'
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

#define CONTROL_TIME_ACTIVADED 1200 //tempo em que o controle se mantém acionado
String controlSwitch;
bool controlState = false;

void buildWebSite() {
  server.send(200, "text/html", webSiteContent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght) {
  String payloadString = (const char*)payload;
  if (type == WStype_TEXT) {
    byte separator = payloadString.indexOf('=');
    String var = payloadString.substring(0, separator);
    String val = payloadString.substring(separator + 1);
    if (var = "controlOn") {
      controlState = false;
      if (val = "ON")
        controlState = true;
    }
  }
}

String tempoLigado() {
  static uint32_t wait;
  if (millis() - wait > 1000) {
  uint32_t duracao = millis();
  uint8_t milissegundos = ((duracao % 1000) / 100);
  uint8_t segundos = ((duracao / 1000) % 60);
  uint8_t minutos = ((duracao / (1000 * 60)) % 60);
  uint8_t horas = ((duracao / (1000 * 60 * 60)) % 24);
  String tempo = "";
  tempo += ((horas < 10) ? "0" : "");
  tempo += String (horas);
  tempo += ((minutos < 10) ? ":0" : ":");
  tempo += String (minutos);
  tempo += ((segundos < 10) ? ":0" : ":");
  tempo += String (segundos);
  return tempo;
  wait = millis();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GATE_PIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println('.');
  }
  server.on("/", buildWebSite);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  static uint32_t controlTimer;
  static uint32_t wait;
  webSocket.loop();
  server.handleClient();
  if (!controlState)
    controlTimer = millis();
  if (millis() - controlTimer > CONTROL_TIME_ACTIVADED)
    controlState = false;

  digitalWrite(GATE_PIN, controlState);
  controlSwitch = "OFF";
  if (controlState) {
    controlSwitch = "ON";
  }
  String JSONtxt = "{\"controlOn\":\"" + controlSwitch + "\", \"timeOn\":\"" + tempoLigado() + "\"}";
  webSocket.broadcastTXT(JSONtxt);
}
