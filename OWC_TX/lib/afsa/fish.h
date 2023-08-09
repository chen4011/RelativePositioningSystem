#ifndef _FISH_H_
#define _FISH_H_

#include "rotMatrix.h"

class fish{
private:
    float obj(matrix pOrg);
    float random(float min, float max);
    matrix prey();
    matrix swarm();
    matrix follow();
    matrix move();
    
public:
    static int tryNum, friendNum;
    static float visual, step, crowdFactor;
    static fish bestFriend;         // The individual that have the smallest fVal
    static matrix pCenter;          // The center position of whole fish swarm

    static float curr[3];
    static rotMatrix rotLED, rotPD;

    static float power, area, responsivity, phiHalf, lambLED, lambPD, s;
    
    matrix pOrg;
    int index;
    float fVal;
    
    fish();
    fish(int index);

    void moveStep();
    
    // void addFriend(fish &af);
    // void statistic();
    // ~fish();
};

#endif