#include "setupInit.h"
#include <WiFi.h>
#include <esp_wifi.h>

// # Wifi setting
const char wifiName[] = "Solab_2G";     // Wi-Fi name 
const char wifiPWD[] = "chanky.123#";   // Wi-Fi password

// # Find the Wi-Fi channel number
int getWiFiChannel(const char *ssid) {
  Serial.print("Find WiFi channel");
  if (int n = WiFi.scanNetworks()) {
      for (int i=0; i<n; i++) {
          Serial.print(".");
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

// # Initialize the Wi-Fi 
void initWiFi(){
  WiFi.mode(WIFI_STA);               // Set ESP32 as a Wi-Fi station(STA)

  // ## Show the Wi-Fi information on the serial monitor
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  int32_t channel = getWiFiChannel(wifiName);
  Serial.println("Success");
  Serial.print("Channel: ");
  Serial.println(channel);
  // WiFi.printDiag(Serial);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  // WiFi.printDiag(Serial);
  
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