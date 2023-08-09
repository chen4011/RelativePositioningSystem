#include "adcRead.h"
#include <Arduino.h>

adcRead::adcRead() {}
adcRead::~adcRead() {}

// # Initialize the ADC1115 and set the DFT parameter
adcRead::adcRead(const float *resistorInput, const float resistorFeedback, const float resistorGround): rf(resistorFeedback), rg(resistorGround) {
    for(int i = 0; i < 3; i++) ri[i] = *(resistorInput + i);

    // ## Initialize the ADC1115
    ads.begin();
    ads.setDataRate(RATE_ADS1115_860SPS);
    ads.setGain(GAIN_TWO);

    // ## Set the DFT parameter
    dftSample = 32;
    dftFrequency = 80;
    idealFrquency = 10;
}

// # Sample the signal of each PD
void adcRead::sample() {
    u_int64_t timeNow = esp_timer_get_time();
    for(int sample = 0; sample < dftSample; sample++) {
        while((esp_timer_get_time() - timeNow) < 1e6/dftFrequency) {}
        timeNow = esp_timer_get_time();
        for(int pin = 0; pin < 3; pin++) {
            signalVoltage[pin][sample] = ads.computeVolts(ads.readADC_SingleEnded(pin));
        }
    }
}

// # Execute the DFT to calculate the volatage from amplifier
void adcRead::dft() {
    float real, img, mag;
    for(int pin = 0; pin < 3; pin++) {
        real = 0;
        img = 0;
        for(int n = 0; n < dftSample; n++) {
            real += signalVoltage[pin][n]* cos(-2* PI* idealFrquency/ dftFrequency* n);
            img += signalVoltage[pin][n]* sin(-2* PI* idealFrquency/ dftFrequency* n);
        }
        mag = 2* sqrt(pow(real,2)+ pow(img,2))/ dftSample;      // mag = 2* norm(real, img)/ N
        voltage[pin] = 2* PI/ 4* mag* 1e3;       // 2*Amplifier = 2* (PI/4)* mag* 1e3[V -> mV]
    }
}

// # Get the sample signal of PD1
void adcRead::getSignal(float (&signalVolt)[32]) {
    for(int sample = 0; sample < dftSample; sample++) {
        signalVolt[sample] = signalVoltage[0][sample]*1e3;
    }
}

// # Calculate the current of each PD
void adcRead::getCurrent(float (&current)[3]) {
    float gain = 1+(rf/rg);
    sample();
    dft();
    for(int pin = 0; pin < 3; pin++) {          // Convert the voltage to the actual current
        current[pin] = voltage[pin]* 1e3/ (gain* ri[pin]);        // μA
    }
}