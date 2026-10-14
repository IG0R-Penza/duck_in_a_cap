#pragma once
#include <vector>

class Matrix{
private:
    int m;
    int n;
    std::vector<double> matrix;
public:
    Matrix();
    Matrix(int m, int n, std::vector<double> matrix);
    Matrix operator*(const Matrix& other) const;
    void set(int i, int j, double value);
    double get(int i, int j) const;
    int getLines();
    int getColumns();
    void normalize();
};
