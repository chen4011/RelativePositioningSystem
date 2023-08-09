#ifndef REMOTEFETCHDATA_WSEVENT_H_
#define REMOTEFETCHDATA_WSEVENT_H_

#include <AsyncWebSocket.h>

// Define the message name
enum wsDataFlag{
    NONE,
    RESET_MPU_PD,
    RESET_MPU_LED,
    CALCU_DIST,
    ESP_RESTART,
    LED_MODE
};

extern wsDataFlag wsFlag;       // Set wsFlag as a global variable

void onSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

#endif