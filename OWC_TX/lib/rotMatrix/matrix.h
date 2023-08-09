#ifndef REMOTEFETCHDATA_MATRIX_H_
#define REMOTEFETCHDATA_MATRIX_H_

class matrix{  
    public:
        
        matrix();
        matrix(int row, int col);
        matrix(float *mat, int row, int col);
        ~matrix();

        matrix(const matrix &that);
        matrix &operator=(const matrix &that);

        float &operator()(int row, int col) const;
        friend const matrix operator*(const matrix &matA, const matrix &matB);
        friend const matrix operator*(const matrix &matA, const float multiplier);
        friend const matrix operator+(const matrix &matA, const matrix &matB);
        friend const matrix operator-(const matrix &matA, const matrix &matB);
        friend const matrix operator/(const matrix &matA, const float divisor);

        void printMat() const;
        void freeMat();
        matrix transpose() const;

        float *oneDimentionMatConvert();

        static float norm(matrix mat);

    private:
        int row;
        int col;
        float **mat;
        void initialize();
};

#endif