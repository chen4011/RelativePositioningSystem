#ifndef REMOTEFETCHDATA_DISTANCE_H_
#define REMOTEFETCHDATA_DISTANCE_H_

float calculateDistance();
void objFuncParamSet(float power, float area, float responsivity, float phiHalf, float psiHalf, float s);
void measuredDataSet(float *current, float *yprLED, float *yprPD);

#endif