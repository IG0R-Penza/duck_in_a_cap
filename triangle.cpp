#include "triangle.h"

Triangle::Triangle(std::vector<double> coors, int color){
    this->initial = Matrix(3, 4, coors);
    this->current = this->initial;
    this->color = color;
}

double Triangle::getAvgZ(){
    return (current.get(0,2)+current.get(1,2)+current.get(2,2))/3;
}

Matrix *Triangle::getInitial(){
    return &initial;
}

Matrix *Triangle::getCurrent()
{
    return &current;
}

void Triangle::setCurrent(Matrix& matrix){
    this->current = matrix;
}

void Triangle::restore(){
    this->current = this->initial;
}

int Triangle::getColor()
{
    return color;
}
