#include <Arduino.h>
#include <Wire.h>
#include "setupInit.h"
#include "adcRead.h"
#include "dmp.h"
#include "I2Cdev.h"
//test
// MAC adrress of TX, which is used on ESP-NOW
uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0x16, 0xE2, 0x68};

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

espNowSendData pdMonitorData;
espNowResetMS resetMS;

// # PD data
const float ri[3] = {98.5E3, 101.4E3, 100.5E3};        // Input resistor: ~100K Ohms
const float rf = 200;          // Feedback resistor: 200 Ohms
const float rg = 100;          // Ground resistor: 100 Ohms
float sample = 50;             // ADC measured sample
adcRead adc;                   // ADC setting and current convert
float current[3] = {1e-4, 1e-4, 1e-4};                 // Store the current data of 3 PDs
float signalVolt[32];          // Voltage samples of PD1

// # MPU6050 ypr angle of PD
TwoWire I2CMPU = TwoWire(1);
bool dmpReady = false;         // Set true if DMP init was successful
float yprDeg[3];               // MPU6050 ypr angle of PD in degrees 
float *yprPD;                  // MPU6050 ypr angle of PD in radian
bool resetMPU = false;         // Set true if user send msg to request PD's MPU6050 reset

// # FreeRTOS program
TaskHandle_t handelTaskMPU;

// # Callback funciton of sending ESP-NOW data
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  // Serial.print("Last Packet Send Status: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// # Callback funciton of receiving ESP-NOW data
void onDataRecv(const uint8_t *macAddr, const uint8_t *data, int dataLen){
  Serial.println("Receive ESP-NOW Data");
  memcpy(&resetMS, data, sizeof(resetMS));
  if(resetMS.resetMPU) resetMPU = true;
  if(resetMS.resetESP) ESP.restart();
}

// # MPU6050 task function
void taskMPU(void *pvParam){
  pdMonitorData.mpuError = false;
  dmpReady = initMPU(&Wire1);
  
  while(1){
    vTaskDelay(5 / portTICK_PERIOD_MS);   // Release the CPU0
 
    if(resetMPU) {                        // User send msg to request PD's MPU6050 reset
      if(!initMPU(&Wire1)){
        dmpReady = false;
      }
      resetMPU = false;
    }

    if(!dmpReady) {                       // Check if DMP status is ready, or send error msg to reset the ESP32
      pdMonitorData.mpuError = true;
      continue;
    }

    yprPD = getYPR();                     // Get ypr data

    if(!yprPD) continue;                  // If get ypr data error, don't convert data to ESP-NOW sent struct
    memcpy(pdMonitorData.ypr, yprPD, 3* sizeof(yprPD));
    // Serial.printf("Task Gyro is at Core %u\n", xPortGetCoreID());
  }
}

void setup() {
  Serial.begin(115200);

  initWiFi();
  initEspNow(onDataSent, onDataRecv, broadcastAddress);
  
  // I2C Setting
  Wire.begin();
  Wire.setClock(400000);   // 400 kHz
  Wire1.begin(19, 18);
  Wire1.setClock(400000);  // 400 kHz

  adc = adcRead(ri, rf, rg);

  xTaskCreatePinnedToCore(taskMPU, "Task MPU", 3000, NULL, 2, &handelTaskMPU, 0);   // Arrange MPU6050 task to core 2
}

void loop() {
  adc.getCurrent(current);
  memcpy(pdMonitorData.current, current, sizeof(current));
  
  adc.getSignal(signalVolt);
  memcpy(pdMonitorData.signalVolt, signalVolt, sizeof(signalVolt));

  // Send current and ypr data to TX via ESP-NOW
  esp_err_t espNowSendResult = esp_now_send(broadcastAddress, (uint8_t *) &pdMonitorData, sizeof(pdMonitorData));
}