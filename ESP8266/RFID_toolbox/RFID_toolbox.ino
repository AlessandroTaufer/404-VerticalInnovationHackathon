#include <Arduino.h>

#include <SPI.h>
#include <MFRC522.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>


#define RST_PIN         D3
#define SS_PIN          D4


MFRC522 mfrc522(SS_PIN, RST_PIN);

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
HTTPClient http;
const char* websockets_server = "ws://SOCKETSERVERIP:8000";

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
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

WebsocketsClient wsClient;

// Tools
#define numberOfTools 3
uint32_t toolUID = 0;
bool toolIsInToolbox = true;
bool firstLoop = true;
int numberOfFalse = 1;


void setup() {
  
  Serial.begin(115200);
  while (!Serial);
  
  SPI.begin();
  mfrc522.PCD_Init();
  delay(10);
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

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
  
  wsClient.ping();
}

void loop() {

  delay(400);

  wsClient.poll();

  if (!mfrc522.PICC_IsNewCardPresent()) { }
  
  if (!mfrc522.PICC_ReadCardSerial()) {

    if (toolIsInToolbox == true) {

      if (++numberOfFalse >= 2) {

        numberOfFalse = 0;
        Serial.println("[TOOLBOX]: tool not found");
        Serial.print("[TOOLBOX]: Tool ");
        Serial.print(toolUID);
        Serial.println(" is NOT in the toolbox.");
        toolIsInToolbox = false;
    
        Serial.println("[TOOLBOX]: sending status update");
        sendStatusUpdate(toolUID, false);
        toolUID = 0;
      }
    }
    
    return;
  }

  numberOfFalse = 0;

  // Print card UID
  Serial.print("=======================");
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  uint32_t uid = 0;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
     uid += (uint32_t) mfrc522.uid.uidByte[i] << ((3-i) * 8);
  }
  Serial.println();
  Serial.println(uid);
  Serial.println("------------------------------");

  if (uid != toolUID) {

    if (!toolIsInToolbox || firstLoop) {
    
      firstLoop = false;
      Serial.print("[TOOLBOX]: Tool ");
      Serial.print(uid);
      Serial.println(" is now in the toolbox.");
      toolUID = uid;
      toolIsInToolbox = true;
    
      Serial.println("[TOOLBOX]: sending status update");
      sendStatusUpdate(uid, true);
    }
  }
}

void sendStatusUpdate(uint32_t UID, boolean isInToolbox) {

  wsClient.send(String(UID) + "," + ((isInToolbox) ? "1" : "0"));
}
