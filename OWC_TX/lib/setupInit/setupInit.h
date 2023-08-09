#ifndef REMOTEFETCHDATA_SETUPINIT_H_
#define REMOTEFETCHDATA_SETUPINIT_H_

#include <AsyncWebSocket.h>
#include <esp_now.h>

extern AsyncWebSocket ws;       //Set ws as a global variable

void initSPIFFS();
void initWiFi();
void initMDNS();
void initServer(AwsEventHandler onSocketEvent);
void initEspNow(esp_now_send_cb_t onDataSend, esp_now_recv_cb_t onDataRecv, uint8_t *broadcastAddress);

#endif