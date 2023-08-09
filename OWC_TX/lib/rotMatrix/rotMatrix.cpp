#include "rotMatrix.h"
#include <math.h>
#include <Arduino.h>

rotMatrix::rotMatrix(){
    mat = matrix(3,3);
}

rotMatrix::rotMatrix(float *ypr){

    matrix tri = matrix(2,3);           // [cosYaw, cosPitch, cosRoll; sinYaw, sinPitch, sinRoll]
    mat = matrix(3,3);

    for(int i = 0; i < 3; i++){
        tri(0, i) = cos(*(ypr+i));
        tri(1, i) = sin(*(ypr+i));
    }

    matrix rotMat[3];                   // [rotX, rotY, rotZ]
    for(int i = 2; i >= 0; i--){
        rotMat[i] = matrix(3,3);
        rotMat[i](i, i) = 1;
        for(int j = 0; j <=2; j++){
            if(i == j)  continue;
            rotMat[i](j, j) = tri(0, 2-i);     //cos

            int k = (j + 1) % 3;
            if(i == k)  continue;
            rotMat[i](j, k) = -tri(1, 2-i); //-sin
            rotMat[i](k, j) = tri(1, 2-i); //sin
            
        }
    }
    mat = rotMat[2] * rotMat[1] * rotMat[0];
}

// rotMatrix::~rotMatrix() {  
    // mat.freeMat();
// }