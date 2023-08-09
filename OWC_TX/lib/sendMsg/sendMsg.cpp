#include "sendMsg.h"
#include <ArduinoJson.h>

// # Send message to client
void sendMsg(const char *type, AsyncWebSocket *ws, float *data, int length) {
  StaticJsonDocument<768> doc;
  char text[500];
  
  doc["type"] = type;
  JsonObject val = doc.createNestedObject("val");
  for(int i = 0; i < length; i++) {
    if(!*(data + i)) break;
    const char *dataIndex = "data" + (char)i;
    val[dataIndex] = *(data + i);
  }
  
  serializeJson(doc, text);
  (*ws).textAll(text);

}
