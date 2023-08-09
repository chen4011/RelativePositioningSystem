#include "afsa.h"
#include <math.h>
#include <Arduino.h>

AFSA::AFSA()
{
    iterMax = 50;
    fValIdeal = 1E-5;

    fish::tryNum = 10;
    fish::friendNum = n;
    fish::visual = 20;
    fish::step = 15;
    fish::crowdFactor = 0.38;

    for(int i = 0; i < n; i++){
        af[i] = fish(i);
    }
    best = fish(-1);
    int index = findBest();
}

fish AFSA::newIter(float &fValBest){
    
    for (int i = 0; i < n; i++) {   // Move to the new position for each AF
        af[i].moveStep();
    }
    int bestAFNextIndex = findBest();
    float fValNext = af[bestAFNextIndex].fVal;
    Serial.printf("bestAFNextIndex = %d, fValNext = %1.5f\n", bestAFNextIndex, fValNext);
    if (fValNext < fValBest) {      // Check if a better result exist in new position
        fValBest = fValNext;
        best = af[bestAFNextIndex];
    }
    if (fValBest < 1) {             // Shorten the step distance of each move when the best result is smaller enough
        fish::step = fValBest * 10;
    }

    return best;
}

int AFSA::findBest(){
    float min = 1000;
    int index = -1;
    matrix pSum = matrix(3,1);
    for(int i = 0; i < n; i++){
        pSum = pSum + af[i].pOrg;
        if(af[i].fVal < min){
            min = af[i].fVal;
            index = i;
        }
    }
    fish::bestFriend = af[index];
    fish::pCenter = pSum / n;
    return index;
}

// void AFSA::findFriend(){
//     for(int i = 0; i < n; i++){
//         for(int j = i+1; j < n ; j++){
//             if(matrix::norm(af[i].pOrg-af[j].pOrg) < visual){
//                 af[i].addFriend(af[j]);
//                 af[j].addFriend(af[i]);
//             }
//         }
//         // af[i].statistic();
//         // cout<<i<<": "<<af[i].fVal<<"\t"<<af[i].friendBestIndex<<"\t"<<af[i].nf<<endl;
//         cout<<i<<": "<<af[i].fVal<<endl;
//     }
// }