#ifndef REMOTEFETCHDATA_ADCREAD_H_
#define REMOTEFETCHDATA_ADCREAD_H_

#include <esp_adc_cal.h>
#include "Adafruit_ADS1X15.h"

class adcRead
{
private:
    Adafruit_ADS1115 ads;

    // # PD data
    float voltage[3];               // Volatge calculated by DFT
    float signalVoltage[3][32];     // Sample volatage
    float ri[3], rf, rg;            // Resistor used in the experiment

    // # DFT parameter
    int dftSample;          // Sample number
    int dftFrequency;       // Sample frequency
    int idealFrquency;      // Signal frequency
    
    void sample();
    void fft();
    void dft();
public:
    adcRead();
    ~adcRead();
    
    adcRead(const float *resistorInput, const float resistorFeedback, const float resistorGround);
    
    void getCurrent(float (&current)[3]);
    void getSignal(float (&signalVolt)[32]);
};

#endif