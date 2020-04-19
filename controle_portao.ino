//o botão pode parar de funcionar caso o aplicativo fique em segundo plano por muito tempo
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include "index.h"
#include "credentials.h"

#define GATE_PIN 5

//Descomendar e inserir os dados do wifi para o esp se conectar
//const char* ssid = "";
//const char* password = "";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(88);

IPAddress staticIP(192, 168, 15, 200); //200 'testes' // 201 'oficial'
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

#define CONTROL_TIME_ACTIVADED 1200 //tempo em que o controle se mantém acionado
String controlSwitch;
bool controlState = false, updateClients = false;

void buildWebSite() {
  server.send(200, "text/html", webSiteContent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght) {
  if (type == WStype_DISCONNECTED) {
    Serial.printf("[%u] Disconnected!\n", num);
  }
  else if (type == WStype_CONNECTED) {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
    updateClients = true;
  }
  else if (type == WStype_TEXT) {
    String payloadString = (const char*)payload;
    byte separator = payloadString.indexOf('=');
    String var = payloadString.substring(0, separator);
    String val = payloadString.substring(separator + 1);
    if (var = "controlOn") {
      controlState = false;
      if (val = "ON")
      Serial.println(val);
        controlState = true;
        updateClients = true;
    }
  }
}

String tempoLigado() {
  uint32_t duracao = millis();
  uint8_t segundos = ((duracao / 1000) % 60);
  uint8_t minutos = ((duracao / (1000 * 60)) % 60);
  uint8_t horas = ((duracao / (1000 * 60 * 60)) % 24);
  uint8_t dias = ((duracao / (1000 * 60 * 60 * 24)));
  String tempo = "";
  tempo += ((dias < 10) ? "0" : "");
  tempo += String (dias);
  tempo += ((horas < 10) ? "d 0" : "d ");
  tempo += String (horas);
  tempo += ((minutos < 10) ? "h 0" : "h ");
  tempo += String (minutos);
  tempo += ((segundos < 10) ? "m 0" : "m ");
  tempo += String (segundos);
  tempo += "s";
  return tempo;
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
  static uint32_t wait1sec;
  webSocket.loop();
  server.handleClient();

  if (!controlState) {
    controlTimer = millis();
  }
  if (millis() - controlTimer > CONTROL_TIME_ACTIVADED) {
    controlState = false;
    updateClients = true;
  }

  digitalWrite(GATE_PIN, controlState);

  controlSwitch = "OFF";
  if (controlState) {
    controlSwitch = "ON";
  }

  if (millis() - wait1sec > 1000) {
    updateClients = true;
    wait1sec = millis();
  }

  if (updateClients) {
    String JSONtxt = "{\"controlOn\":\"" + controlSwitch + "\", \"timeOn\":\"" + tempoLigado() + "\"}";
    Serial.println(tempoLigado());
    webSocket.broadcastTXT(JSONtxt);
    updateClients = false;
    wait1sec = millis();
  }
}
