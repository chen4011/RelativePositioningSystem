# Introduction
## OWC_TX
The ESP32 MCU code of transmitter(TX)
### Feature
- LED modulation
- Communication with receiver(RX) via ESP-NOW
- Web server using WebSocket communication protocal
- Angle measurement
- 3-D relative position calculation

## OWC_RX
The ESP32 MCU code of receiver(RX)
### Feature
- Photodiode(PD) signal analysis using Discrete Fourier Transform(DFT)
- Built-in Analog-to-digital converter(ADC) replacement
- Communication with transmitter(TX) via ESP-NOW
- Angle measurement

## ParScript
DENSO Robot Arm control code
### Feature
- Rotation along specific axis
- Move to each reference point
- Move along the path

# Basic setting
## Wi-Fi
Change the Wi-Fi name and password in "lib\setupInit\setupInit.cpp", use the device under the same Wi-Fi.

## ESP-NOW
Check the MAC address of two ESP32, and edit the "broadcastAddress" in "src\main.cpp" to the peer's MAC address.

## Web server
Upload the web source in "data\www\" to the ESP32 file system, there are two methods to upload the files:
- Arduino IDE: go to "Tools > ESP32 Data Sketch Upload"
- PlatformIO extension of vscode: switch to the extension in side page, and go to "Platform > Upload Filesystem Image"

## I2C
MPU6050 and ADS1115 of RX are using I2C protocol to communication with ESP32, set the ADS1115 connect to the GPIO pin 21 and 22, and MPU6050 connect to the GPIO pin 19 and 18.

# Web page UI Feature
- Web server connection status
- Upload firmware via OTA
- Change the LED shine mode
- Reset the IMU of TX and RX
- Record data an copy to clipboard
- Calculate the relative position