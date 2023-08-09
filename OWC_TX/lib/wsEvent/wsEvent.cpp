#include "wsEvent.h"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
// #include <AsyncWebSocket.h>
#include <ArduinoJson.h>
// #include "dmp.h"
// #include "sendMsg.h"



wsDataFlag wsFlag = NONE;   // The message receive from web socket

// # Callback function that will run whenever we receive new data from the clients via WebSocket protocol
void wsGetData(char *data, AsyncWebSocket *ws){
  StaticJsonDocument<64> doc;

  // ## Deserialize the JSON data
  DeserializationError error = deserializeJson(doc, data);
  if(error){
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // ## Change the wsFlag according to the receiving message
  const char *type = doc["type"];
  if(strcmp(type, "resetMPUPD") == 0){
    wsFlag = RESET_MPU_PD;
  } else if(strcmp(type, "resetMPULED") == 0){
    wsFlag = RESET_MPU_LED;
  } else if(strcmp(type, "calcuDist") == 0){
    wsFlag = CALCU_DIST;
  } else if(strcmp(type, "espRestart") == 0){
    wsFlag = ESP_RESTART;
  } else if(strcmp(type, "ledMode") == 0){
    wsFlag = LED_MODE;
  }
  
}

// # Configure an event listener to handle the different asynchronous steps of the WebSocket protocol
void onSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  switch (type)
  {
    case WS_EVT_CONNECT:    // When a client has logged in
      Serial.printf("Client(IP: %s, ID: %u) connected\n", client->remoteIP().toString().c_str(), client->id());
      break;
    case WS_EVT_DISCONNECT: // When a client has logged out
      Serial.println("Client disconnected");
      break;
    case WS_EVT_DATA:       // When a data packet is received from the client
      wsGetData((char *)data, server);
      // height = atof((char *)data);
      break;
    case WS_EVT_PONG:       // In response to a ping request
      Serial.println("Server send ping msg");
      break;
    default:
      break;
  }
}