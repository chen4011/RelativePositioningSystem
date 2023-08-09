#ifndef _AFSA_H_
#define _AFSA_H_

#include "fish.h"

class AFSA
{
private:
    int n = 20;
    fish af[20], best;
    
    // void findFriend();
public:
    AFSA();
    float fValIdeal;
    int iterMax;

    fish newIter(float &fValBest);
    int findBest();
    // ~AFSA();
};

#endif