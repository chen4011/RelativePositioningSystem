#include "distance.h"
#include "afsa.h"
#include "fish.h"
#include "rotMatrix.h"
#include "sendMsg.h"
#include "setupInit.h"
#include <time.h>
#include <math.h>
#include <limits.h>

float calculateDistance() {
    srand (static_cast <unsigned> (time(0)));       // Set the random function to "real" random

    AFSA algorithm = AFSA();
    float fValBest = std::numeric_limits<float>::max();          // Get the minimum objective function value of current fish swarm
    int iter = 0;       // Record iteration time of algorithm
    float dist = 0;     // Distance between TX and RX
    fish bestAF;        // Best artificial fish(AF) in all iteration

    // Iteration stop when minimum objective function value is smaller than ideal value or iteration time reach the limit
    while (fValBest > algorithm.fValIdeal && iter++ < algorithm.iterMax) {
        bestAF = algorithm.newIter(fValBest);                           // Get the best AF
        Serial.printf("iter: %d, Best fVal: %1.5f\n", iter, fValBest);
        bestAF.pOrg.printMat();                                         // Print the position of best AF
        sendMsg("af", &ws, bestAF.pOrg.oneDimentionMatConvert(), 3);    // Send the position of best AF to web socket
    }
    
    dist = matrix::norm(bestAF.pOrg);       // Caluculate the distance via norm of best AF in all iteration
    
    return dist;
}

void objFuncParamSet(float power, float area, float responsivity, float phiHalf, float psiHalf, float s) {
    fish::power = power;
    fish::area = area;
    fish::responsivity = responsivity;
    fish::phiHalf = phiHalf;
    fish::lambLED = -log(2)/ log(cos(phiHalf * M_PI / 180));
    fish::lambPD = -log(2)/ log(cos(psiHalf * M_PI / 180));
    fish::s = s;

    Serial.printf("power: %1.5f, area: %1.5f, response: %f, m: %f, M: %f\n", fish::power, fish::area, fish::responsivity, fish::lambLED, fish::lambPD);
}

void measuredDataSet(float *current, float *yprLED, float *yprPD) {
    fish::rotLED = rotMatrix(yprLED);
    fish::rotPD = rotMatrix(yprPD);
    fish::rotLED.mat.printMat();
    fish::rotPD.mat.printMat();

    for(int i = 0; i < 3; i++) {
        fish::curr[i] = *(current+i);
        Serial.printf("curr[%d]: %1.5f, ", i, fish::curr[i]);
    }
    Serial.println();
}