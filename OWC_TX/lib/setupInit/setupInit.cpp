#include "setupInit.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// # Async web socket
AsyncWebServer server(80);              // Create an AsyncWebServer object on port 80
AsyncWebSocket ws("/ws");               // Create an AsyncWebSocket object to handle the connections on the /ws path

// # Wifi setting
const char wifiName[] = "Solab_2G";     // Wi-Fi name 
const char wifiPWD[] = "chanky.123#";   // Wi-Fi password

// # Initialize the ESP32 file system 
void initSPIFFS(){
  if(!SPIFFS.begin(true)){
    Serial.printf("Error initial SPIFFS\n");
    while(1){ }
  }
}

// # Initialize the Wi-Fi 
void initWiFi(){
  WiFi.mode(WIFI_AP_STA);               // Set ESP32 as a Wi-Fi access point(AP) and station(STA), which can be used in ESP-NOW and web server
  WiFi.begin(wifiName, wifiPWD);        // Start the Wi-Fi connection
  
  Serial.print("\nConnect to WiFi");

  while(WiFi.status() != WL_CONNECTED){ // Wait until connected to Wi-Fi
   Serial.print(".");
   delay(500);
  }

  Serial.printf("Scuccess\n");

  // ## Show the Wi-Fi information on the serial monitor
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("MAC channel: ");
  Serial.println(WiFi.channel());
}

// # Initialize the mDNS(multicast Domain Name System)
void initMDNS(){
  if(!MDNS.begin("OWCBS")){   // Set the user defined domain name, and check the construction result
    Serial.println("mDNS construction failed");
    Serial.println("Restart plz");
    while(1) {      
      delay(1000); // mDNS construction failed, program stop
    }
  }
  Serial.println("mDNS construction success");
}

// # Update firmware via OTA(On The Air)
void handleUpdate(AsyncWebServerRequest *req, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if(!index) {
    Serial.printf("Update firmware: %s\n", filename.c_str());

    if(!Update.begin()) {
      Update.printError(Serial);
    }
  }

  if(len) {
    Update.write(data, len);
  }
  
  if(final) {
    if(Update.end(true)) {
      Serial.printf("Wrote %u bytes\nRebooting", index + len);
    } else {
      Update.printError(Serial);
    }
  }
}

// # Initialize the web server
void initServer(AwsEventHandler onSocketEvent){
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");  // Set the home page
  server.serveStatic("/ota", SPIFFS, "/www/ota.html");                    // Set the OTA page
  server.serveStatic("/img", SPIFFS, "/www/img/");                        // Set the image folder
  server.serveStatic("/favicon.ico", SPIFFS, "/www/favicon.ico");         // Set the website icon

  // Process the firmware upload via OTA when receive a request from the user, and restart the ESP32 after update to new firmware
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *req) {        
    ESP.restart();
  }, handleUpdate);

  // ## Initialize the WebSocket protocol
  ws.onEvent(onSocketEvent);
  server.addHandler(&ws);

  server.begin();                               // Start the server

  MDNS.setInstanceName("Vivian's ESP32");
  MDNS.addService("http", "tcp", 80);
}

// # Initialize the ESP-NOW
void initEspNow(esp_now_send_cb_t onDataSend, esp_now_recv_cb_t onDataRecv, uint8_t *broadcastAddress){
  esp_now_peer_info_t peerInfo;

  while(esp_now_init() != ESP_OK){              // Start the ESP-NOW, and check the status
    Serial.println("Error initializing ESP-NOW");
  }

  esp_now_register_send_cb(onDataSend);         // Set the callback function of sending ESP-NOW data

  // ## Create the ESP-NOW peer information
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){   // Add peer
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);         // Set the callback function of receiving ESP-NOW data
}

