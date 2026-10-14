#include "matrix.h"
Matrix::Matrix() {
    this->m = 0;
    this->n = 0;
    this->matrix = std::vector<double>();
}
Matrix::Matrix(int m, int n, std::vector<double> matrix){
    this->m = m;
    this->n = n;
    this->matrix = matrix;
}

double Matrix::get(int i, int j) const{
    return matrix[i*n+j];
}

void Matrix::set(int i, int j, double value){
    matrix[i*n+j] = value;
}

Matrix Matrix::operator*(const Matrix& other) const{
    Matrix res = Matrix(m, n, std::vector<double>(m * n, 0));
    for (int i1 = 0; i1 < m; i1++){
        for (int i2 = 0; i2 < other.m; i2++){
            double sum = 0;
            res.set(i1, i2, 0);
            for (int j = 0; j < n; j++){
                sum += get(i1, j)*other.get(i2, j);
            }
            res.set(i1, i2, sum);
        }
    }
    return res;
}

int Matrix::getLines() {return m;}
int Matrix::getColumns() {return n;}

void Matrix::normalize() {
    for (int i = 0; i < m; i++){
        for (int j = 0; j < n; j++){
            if (get(i, n-1)<=1e-6 || get(i, n-1)!=get(i, n-1)){
                set(i, n-1, 0.0);
                continue;
            }
            set(i, j, get(i, j)/get(i, n-1));
        }
    }
}