#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>


ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
HTTPClient http;
const char* websockets_server = "ws://SOCKETSERVERIP:8001";

using namespace websockets;

WebsocketsClient wsClient;

bool isBuzzerOn = false;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    isBuzzerOn = true;
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void setup() {
  
  Serial.begin(115200);
  while (!Serial);
  WiFi.mode(WIFI_STA);
  WiFi.begin("WIFIAPNAME", "WIFIPSK");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  wsClient.onMessage(onMessageCallback);
  wsClient.onEvent(onEventsCallback);

  bool connected = false;
  do {
    yield();
    connected = wsClient.connect(websockets_server);
  } while (!connected);
  delay(500);

  wsClient.send("buzzer");
  
  wsClient.ping();

  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
}

void loop() {

  wsClient.poll();

  delay(50);
  
  if (isBuzzerOn) {
    isBuzzerOn = false;
    analogWrite(D4, 128);
    tone(D5, 2);

    delay(3500);

    analogWrite(D4, 0);
  }
}
