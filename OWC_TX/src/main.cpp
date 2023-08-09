#include <Arduino.h>
#include <esp_now.h>
#include <Wire.h>
#include "setupInit.h"
#include "wsEvent.h"
#include "sendMsg.h"
#include "distance.h"
#include "dmp.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


// MAC adrress of RX, which is used on ESP-NOW
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0xA6, 0xA8, 0xAC};

// # PD Monitor Data
typedef struct espNowSendData {
  float current[3];       // Current of 3 PDs
  float signalVolt[32];   // Voltage samples of PD1
  float ypr[3];           // Angle of RX
  bool mpuError;          // MPU initialization error message
} espNowSendData;

// # PD Reset Notification
typedef struct espNowResetMS {
  bool resetMPU;          // Message to request RX's MPU6050 reset
  bool resetESP;          // Message to request RX's ESP32 reset
} espNowResetMS;

// # PD data
espNowSendData pdMonitorData;
espNowResetMS resetMS;
float current[3] = {1e-4, 1e-4, 1e-4};      // PD's Cuurent
float signalVolt[32];           // Voltage samples of PD1
float yprPD[3] = {0, 0, 0};     // MPU6050 ypr angle of PD [rad]
float yprPDDeg[3] = {0, 0, 0};  // MPU6050 ypr angle of PD [deg]

// # MPU6050 ypr angle of LED
bool dmpReady = false;          // set true if DMP init was successful
float *yprNow;
float yprLED[3] = {0, 0, 0};    // MPU6050 ypr angle of LED [rad]
float yprLEDDeg[3] = {0, 0, 0}; // MPU6050 ypr angle of LED [deg]
int cumulTime = 0;              // To prevent stack overflow, after execute getYPR() 50 times, send msg to ws

// # Distance
bool startCalcu = false;        // Record the calcualation start, stop the ypr angle record

// # LED Blink
int pinLED = 23;
int mode = 2;

// # Objective function parameter
float power = 16.1;             // Radiant power of LED [mW]
float area = 7.5;               // Radiant sensitive area of PD [mm^2]
float responsivity = 0.5777;    // Responsivity of PD [mA/mW]
float phiHalf = 60;             // Angle of half sensitivity of LED [deg]
float psiHalf = 65;             // Angle of half sensitivity of PD  [deg]
float s = 2.608;                // Exponential angle gain parameter

// # FreeRTOS program
TaskHandle_t handelTaskMPU;

// # Callback funciton of sending ESP-NOW data
void onDataSend(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// # Callback funciton of receiving ESP-NOW data
void onDataRecv(const uint8_t *macAddr, const uint8_t *data, int dataLen){
  memcpy(&pdMonitorData, data, sizeof(pdMonitorData));
  
  memcpy(current, pdMonitorData.current, sizeof(current));
  memcpy(signalVolt, pdMonitorData.signalVolt, sizeof(signalVolt));
  memcpy(yprPD, pdMonitorData.ypr, sizeof(yprPD));
  if(pdMonitorData.mpuError) {
    sendMsg("resetESP32", &ws, nullptr, 0);
  }
}

// # MPU6050 task function
void taskMPU(void *pvParam){
  Wire.begin();
  Wire.setClock(400000);  // 400 kHz
  dmpReady = initMPU();
  
  while(1){
    vTaskDelay(5 / portTICK_PERIOD_MS);   // Release the CPU0
 
    // User send msg to request LED's MPU6050 reset
    if(wsFlag == RESET_MPU_LED) {
      if(!initMPU()){
        dmpReady = false;
      }
      wsFlag = NONE;
      cumulTime = 0;
    }
    
    // Check if DMP status is ready, or send error msg to reset the ESP32
    if(!dmpReady) {
      sendMsg("resetESP32", &ws, nullptr, 0);
      continue;
    }

    yprNow = getYPR();        // Get ypr data

    // If get ypr data error or start distance calculation, stop convert radian to degree and send data to ws
    if(!yprNow || startCalcu) continue;

    // To prevent stack overflow, after execute getYPR() 50 times, send msg to ws
    if(++cumulTime == 200){

      for(int i = 0; i<3; i++){
        yprLED[i] = *(yprNow + i);
        yprLEDDeg[i] = *(yprLED + (2-i)) * 180 / M_PI;    // Convert radian to degree
        sendMsg("rotLED", &ws, (float *)yprLEDDeg, 3);    // Send LED's ypr in degree to ws
      }
      cumulTime = 0;
    }
  }
}

void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  initSPIFFS();
  initWiFi();
  initMDNS();
  initServer(onSocketEvent);
  initEspNow(onDataSend, onDataRecv, broadcastAddress);
  
  xTaskCreatePinnedToCore(taskMPU, "Task MPU", 3000, NULL, 2, &handelTaskMPU, 0);      // Arrange MPU6050 task to core 2
  ledcSetup(0, 10, 8);        // Set LED blink frequency
  ledcAttachPin(pinLED, 0);   // Set LED GPIO Number

  for(int i = 0; i <32; i++) {
    signalVolt[i] = 0;
  }

}

void loop() {
  ws.cleanupClients();

  // # Change the LED shine mode via PWM
  switch (mode)
  {
    case 0:    // No light
      ledcWrite(0, 0);
      break;
    case 1:   // Shine without blink
      ledcWrite(0, 255);
      break;
    case 2:  // Shine with blink in setting frequency
      ledcWrite(0, 127);
      break;
    default:
      break;
  }

  // # Check the message that user send to web socket
  switch (wsFlag)
  {
    case RESET_MPU_PD:    // Request to reset the MPU6050 of RX
      resetMS.resetMPU = true;
      esp_now_send(broadcastAddress, (uint8_t *) &resetMS, sizeof(resetMS));
      resetMS.resetMPU = false;
      delay(3000);        // Wait for RX's MPU6050 reset
      break;
    case LED_MODE:        // Change the LED shine mode
      mode++;
      mode %= 3;
      Serial.println(mode);
      break;
    case CALCU_DIST:      // Calculate the distance between TX ans RX
    {
      startCalcu = true;
      float result;
      float currentIdeal[3] = {4.70643903143176, 2.95959962604814, 2.95959962604814};
      float yprLEDIdeal[3] = {0, 0, 180 * M_PI / 180};
      float yprPDIdeal[3] = {0, 0, 0};
      objFuncParamSet(power, area, responsivity, phiHalf, psiHalf, s);   // Set the parameter of LED and PD
      measuredDataSet((float *)currentIdeal, (float *)yprLEDIdeal, (float *)yprPDIdeal);   // Set the measured data(current, angle(ideal))
      // measuredDataSet((float *)current, (float *)yprLED, (float *)yprPD);   // Set the measured data(current, angle(actual))
      result = calculateDistance();                   // Start calculation
      Serial.printf("distance = %1.5f", result);
      sendMsg("distCalcuFinish", &ws, &result, 1);    // Send the calculation result to web socket
      startCalcu = false;
      break;
    }
    case ESP_RESTART:     // Request to reset the ESP32 of TX and RX
      resetMS.resetESP = true;
      esp_now_send(broadcastAddress, (uint8_t *) &resetMS, sizeof(resetMS));
      resetMS.resetESP = false;
      delay(3000);        // Wait for PD's ESP32 reset
      ESP.restart();
    default:
      break;
  }
  wsFlag = NONE;          // Clear the message

  for(int i = 0; i<3; i++){
    yprPDDeg[i] = yprPD[2-i] * 180 / M_PI;    // Convert PD's ypr from radian to degree
  }
  sendMsg("current", &ws, (float *)current, 3);         // Send PD's cuurent to web socket
  // sendMsg("signal", &ws, (float *)signalVolt, 16);      // Send PD's voltage sample to web socket (Used to check LED blink and DFT function)
  sendMsg("rotPD", &ws, (float *)yprPDDeg, 3);          // Send PD's ypr in degree to web socket

  delay(1000);
}