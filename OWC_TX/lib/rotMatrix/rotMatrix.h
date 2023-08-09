#ifndef REMOTEFETCHDATA_ROTMATRIX_H_
#define REMOTEFETCHDATA_ROTMATRIX_H_

#include "matrix.h"

class rotMatrix{
    public: 
        matrix mat;
        rotMatrix();
        rotMatrix(float *ypr);
        // ~rotMatrix();
    private:
        // matrix tri;
};

#endif