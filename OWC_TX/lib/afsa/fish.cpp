#include "fish.h"
#include <math.h>
#include <Arduino.h>

// ------------------------------------------------------------ //
// Static member is declared in class scope,                    // 
// but definition is performed at file scope,                   //
// i.e., MUST be defined outside the class declaration.         //

// # AFSA parameter
int fish::tryNum;                       // Max try number to find a random direciton
int fish::friendNum;                    // The number to artificial fish
float fish::visual;                     // Visual length of fish
float fish::step;                       // Move length in one step  of fish
float fish::crowdFactor;                // Maximum crowd level of fish
fish fish::bestFriend;                  // The individual that have the smallest fVal
matrix fish::pCenter;                   // The center position of whole fish swarm

// # Measured data
rotMatrix fish::rotLED, fish::rotPD;    // Rotation matrix of LED and PD
float fish::curr[3];                    // Current of four PDs

// # Objective function parameter
float fish::power;                      // Radiant power of LED [mW]
float fish::area;                       // Radiant sensitive area of PD [cm^2]
float fish::responsivity;               // Responsivity of PD [A/W]
float fish::phiHalf;                    // Half angle of LED
float fish::lambLED;                    // Lambertian order of LED
float fish::lambPD;                     // Lambertian order of PD
float fish::s;

// ------------------------------------------------------------ //

fish::fish(): fVal(0) { }

fish::fish(int index): index(index) {       // Randomly set the position of artificial fish(AF)
    pOrg = matrix(3,1);
    for (int i = 0; i < 2; i++) {
        pOrg(i,0) = random(-200, 200);      // AF Coordinate of x and y, Unit: mm 
    }
    pOrg(2,0) = random(0, 200);             // AF Coordinate of z, Unit: mm
    fVal = obj(pOrg);                       // Calculate the objective funciton value
}

void fish::moveStep() {                     // Move to the next position via best method(prey, swarm, follow and random)
    float fValMax = fVal;                   // The best objective function value in three methods
    int indexMax = -1;                      // Record the best method number, and check if not found the best value from three methods

    float fValNext;                         // The objective function value of next position
    matrix pOrgNext[3];                     // Record the position of each method

    for (int i = 0; i < 3; i++){
        
        switch (i)
        {
            case 0:
                pOrgNext[i] = prey();
                break;
            case 1:
                pOrgNext[i] = swarm();
                break;
            case 2:
                pOrgNext[i] = follow();
                break;
            default:
                break;
        }
        fValNext = obj(pOrgNext[i]);
        if (fValNext < fValMax) {
            fValMax = fValNext;
            indexMax = i;               // Record the best method number
        }
    }

    if (indexMax != -1) {               // Move to the best next position
        pOrg = pOrgNext[indexMax];
        fVal = fValMax;
    } else {                            // If not found the best from three methods, random move to a position
        pOrg = move();
        fVal = obj(pOrg);
    }
}

matrix fish::prey() {                   // Move toward the high concentration of food
    int time = 0;
    matrix pVisual = matrix(3,1);

    while (time++ < tryNum) {
        for (int i = 0; i < 3; i++) {
            pVisual(i, 0) = pOrg(i, 0) + visual * random(-1, 1);
        }
        if (obj(pVisual) < fVal) {
            break;
        }
    }
    if (time != tryNum +1) {
        return pOrg + (pVisual - pOrg) / matrix::norm(pVisual - pOrg) * (step * random(0, 1));
    } else {
        return move();
    }
}

matrix fish::swarm() {                  // Swarm with friend
    if ((obj(pCenter)/friendNum) < (crowdFactor * fVal)) {
        return pOrg + (pCenter - pOrg) / matrix::norm(pCenter - pOrg) * step;
    } else {
        return prey();
    }
}

matrix fish::follow() {                 // Follow the best AF
    if (bestFriend.fVal/friendNum < crowdFactor * fVal) {
        return pOrg + (bestFriend.pOrg - pOrg) / matrix::norm(bestFriend.pOrg - pOrg) * step;
    } else {
        return prey();
    }
}

matrix fish::move() {                   // Random move to a position
    matrix pNext = matrix(3,1);

    for (int i = 0; i < 3; i++) {
        pNext(i, 0) = pOrg(i, 0) + step * random(-1, 1);
    }
    return pNext;
}

float fish::obj(matrix L_P1) {          // Objective function of optimization
    matrix L_Pi[3], Pi_L[3], n_L, n_P;  // Position of 3 PDs, and normal vector of LED and PD
    float length = 38.1;                // Spacing between PD1 and PD2 (Use the same spacing between PD1 and PD3 in this experiment)

    for (int i = 0; i < 3; i++) {       // Calculate the position of each PD in coordinate system {L}, which is origin of TX
        L_Pi[i] = matrix(3, 1);         // Set the position of each PD in coordinate system {P}, which is origin of RX
        
        switch (i)
        {
        case 1:
            L_Pi[1](0, 0) = length;     // PD2 position
            break;
        case 2:
            L_Pi[2](1, 0) = length;     // PD3 position
            break;
        default:
            break;
        }
        L_Pi[i] = L_P1 + (rotLED.mat.transpose() * (rotPD.mat * L_Pi[i]));   // Position representation from {P} to {L}
        Pi_L[i] = (rotPD.mat.transpose() * -1) * (rotLED.mat * L_Pi[i]);
    }

    n_L = matrix(3, 1);
    n_L(2, 0) = 1;

    n_P = matrix(3, 1);
    n_P(2, 0) = 1;

    float d, phi, psi, k, idealI, err = 0;
    for (int i = 0; i < 3; i++) {
        d = matrix::norm(L_Pi[i]);
        phi = acos((n_L(0,0) * L_Pi[i](0,0) + n_L(1,0) * L_Pi[i](1,0) + n_L(2,0) * L_Pi[i](2,0))/d);
        psi = acos((n_P(0,0) * Pi_L[i](0,0) + n_P(1,0) * Pi_L[i](1,0) + n_P(2,0) * Pi_L[i](2,0))/d);
        k = -pow(phiHalf, s)/log(2);
        idealI = power * (lambLED + 1) * area * responsivity * exp(pow(phi*180/M_PI, s)/k) * pow(cos(psi), lambPD) / (2 * M_PI * pow(d, 2)) * 1e3;
        err += abs(curr[i] - idealI);
    }
    return err;
}

float fish::random(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max-min);
}

// void fish::addFriend(fish &af){
//     friendAF.push(af.index);
// }

// void fish::statistic(){
//     nf = friendAF.size();
//     matrix pSum = matrix(3,1);
//     float fitBest = fVal;
//     fish afNow;
//     for(int i = 0; i < nf; i++){
//         // afNow = friendAF.front();
//         // pSum = pSum + afNow.pOrg;
//         // if(afNow.fVal < fitBest){
//         //     fitBest = afNow.fVal;
//         //     friendBestIndex = afNow.index;
//         // }
//         // friendAF.pop();
//     }
//     pSwarm = pSum/nf;
// }

// fish::~fish()
// {
// }
