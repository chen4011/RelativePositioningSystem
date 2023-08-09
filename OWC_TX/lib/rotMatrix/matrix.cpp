#include "matrix.h"
#include <Arduino.h>
// #include <esp_heap_caps.h>

// matrix matrix::matNew;

matrix::matrix() {
    this->row = 1;
    this->col = 1;

    initialize();
}

matrix::matrix(int row, int col) {
    this->row = row;
    this->col = col;
    
    initialize();
}

matrix::matrix(float *mat, int row, int col) {
    this->row = row;
    this->col = col;

    initialize();
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            this->mat[i][j] = mat[i*col+j];
        }
    }
}

matrix::matrix(const matrix &that) {
    row = that.row;
    col = that.col;
    initialize();

    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            this->mat[i][j] = that.mat[i][j];
        }
    }
}
        
matrix &matrix::operator=(const matrix &that) {
    if (this != &that) {
        freeMat();

        row = that.row;
        col = that.col;
        initialize();

        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                this->mat[i][j] = that.mat[i][j];
            }
        }
    }

    return *this;

}

float &matrix::operator()(int row, int col) const {
    if(row >= (*this).row || col >= (*this).col)  
        exit(1);
    return mat[row][col];
}

const matrix operator*(const matrix &matA, const matrix &matB) {
    matrix matC = matrix(matA.row, matB.col);
    
    // matNew = matrix(matA.row, matB.col);
    // if(matA.col != matB.row) return matC;

    for(int i = 0; i < matA.row; i++){
        for(int j = 0; j < matB.col; j++){
            for(int k = 0; k < matA.col; k++){
                matC.mat[i][j] += matA.mat[i][k] * matB.mat[k][j];
            }
        }
    }
    return matC;
}

const matrix operator*(const matrix &matA, const float multiplier) {
    matrix matC = matrix(matA.row, matA.col);

    for(int i = 0; i < matA.row; i++){
        for(int j = 0; j < matA.col; j++){
            matC.mat[i][j] = matA.mat[i][j] * multiplier;
        }
    }

    return matC;
}

const matrix operator+(const matrix &matA, const matrix &matB) {
    matrix matC = matrix(matA.row, matA.col);

    if(matA.row != matB.row && matA.col != matB.col) return matC;

    for(int i = 0; i < matA.row; i++){
        for(int j = 0; j < matA.col; j++){
            matC.mat[i][j] = matA.mat[i][j] + matB.mat[i][j];
        }
    }

    return matC;
}

const matrix operator-(const matrix &matA, const matrix &matB) {
    matrix matC = matrix(matA.row, matA.col);

    if(matA.row != matB.row && matA.col != matB.col) return matC;

    for(int i = 0; i < matA.row; i++){
        for(int j = 0; j < matA.col; j++){
            matC.mat[i][j] = matA.mat[i][j] - matB.mat[i][j];
        }
    }

    return matC;
}

const matrix operator/(const matrix &matA, const float divisor) {
    matrix matC = matrix(matA.row, matA.col);

    for(int i = 0; i < matA.row; i++){
        for(int j = 0; j < matA.col; j++){
            matC.mat[i][j] = matA.mat[i][j] / divisor;
        }
    }

    return matC;
}

void matrix::printMat() const {
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            Serial.print(mat[i][j]);
            Serial.print("\t");
        }
        Serial.print("\n");
    }
    Serial.print("\n");
    
}

void matrix::freeMat() {
    // Serial.println("freeMat");
    if (mat == NULL) return;

    for(int i = 0; i < row; i++) {
        if (mat[i] == NULL) return;
        free(mat[i]);
    }
    free(mat);
    mat = NULL;
}

// void matrix::resetMat() {
//     for (int i = 0; i < row; i++) {
//         for (int j = 0; j < col; j++) {
//             mat[i][j] = 0;
//         }
//     }
// }

matrix matrix::transpose() const {
    matrix matTrans = matrix(col, row);
    for(int i = 0; i < matTrans.row; i++){
        for(int j = 0; j < matTrans.col; j++){
            matTrans.mat[i][j] = mat[j][i];
        }
    }
    return matTrans;
}

float *matrix::oneDimentionMatConvert() {
    if(col == 1) {
        float *value = (float*)malloc(sizeof(float) * row);
        for(int i = 0; i < row; i++) {
            *(value+i) = mat[i][0];
        }
        return value;
    } else if(row == 1) {
        float *value = (float*)malloc(sizeof(float) * col);
        for(int i = 0; i < col; i++) {
            *(value+i) = mat[0][i];
        }
        return value;
    } else {
        return 0;
    }
}

void matrix::initialize() {
    mat = (float**) malloc(sizeof(float*) * row);   // C
    
    // mat = (float**) heap_caps_malloc(sizeof(float*) * row, MALLOC_CAP_8BIT);   // ESP-IDF
    // mat = new float*[row];   // C++
    
    if(mat == nullptr) return;
    for(int i = 0; i < row; i++){
        mat[i] = (float*) calloc(col, sizeof(float));   // C

        // mat[i] = (float*) heap_caps_calloc(col, sizeof(float), MALLOC_CAP_8BIT);   // ESP-IDF

        // mat[i] = new float[col];     // C++
        // for (int j = 0; j < col; j++) {
        //     mat[i][j] = 0;
        // }

        if(mat[i] == nullptr) return;
    }
}

float matrix::norm(matrix mat){
    float cumul = 0;
    for(int i = 0; i < mat.row; i++){
        cumul += pow(mat.mat[i][0],2);
    }
    return sqrt(cumul);
}

matrix::~matrix(){

    if(mat == nullptr) return;
    for(int i = 0; i < row; i++){
        if (mat[i] == nullptr) return;
        free(mat[i]);
    }
    free(mat);
    mat = NULL;
}
