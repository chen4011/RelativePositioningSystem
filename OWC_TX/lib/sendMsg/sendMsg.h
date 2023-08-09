#ifndef REMOTEFETCHDATA_SENDMSG_H_
#define REMOTEFETCHDATA_SENDMSG_H_

#include <AsyncWebSocket.h>

void sendMsg(const char *type, AsyncWebSocket *ws, float data[], int length);

#endif