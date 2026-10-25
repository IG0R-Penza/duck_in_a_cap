#pragma once

#include "matrix.h"

class Triangle {
private:
    Matrix initial;
    Matrix current;
    int color;

public:
    Triangle(std::vector<double> coors, int color);
    double getAvgZ();

    Matrix* getInitial();
    Matrix* getCurrent();
    void setCurrent(Matrix& matrix);
    void restore();
    int getColor();
    //std::pair<std::vector<std::pair<double, double>>, std::vector<std::pair<double, double>>> get();
};
