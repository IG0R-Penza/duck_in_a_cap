#pragma once

#include "matrix.h"
#include "math.h"

class Triangle {
private:
    Matrix initial;
    Matrix current;
    int color;

public:
    Triangle(std::vector<double> coors, int color);
    double getAvgZ();
    double getEdgeLen(int a, int b);
    std::pair<Triangle, Triangle> split();
    double getLongestLen();

    Matrix* getInitial();
    Matrix* getCurrent();
    void setCurrent(Matrix& matrix);
    void restore();
    int getColor();
};
