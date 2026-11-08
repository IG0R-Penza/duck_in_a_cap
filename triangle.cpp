#include "triangle.h"

Triangle::Triangle(std::vector<double> coors, int color){
    this->initial = Matrix(3, 4, coors);
    this->current = this->initial;
    this->color = color;
}

double Triangle::getAvgZ(){
    return (current.get(0,2)+current.get(1,2)+current.get(2,2))/3;
}

double Triangle::getEdgeLen(int a, int b)
{
    double dx = current.get(a, 0) - current.get(b, 0);
    double dy = current.get(a, 1) - current.get(b, 1);
    double dz = current.get(a, 2) - current.get(b, 2);
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

std::pair<Triangle, Triangle> Triangle::split()
{
    int a = 0;
    int b = 1;
    int c = 2;

    if (getEdgeLen(a, b) < getEdgeLen(0, 2)) {b = 2; c = 1;}
    if (getEdgeLen(a, b) < getEdgeLen(1, 2)) {c = 0; a = 1; b = 2;}

    return std::pair<Triangle, Triangle>(
        Triangle(
            std::vector<double>({
                current.get(a, 0), current.get(a, 1), current.get(a, 2), 1,
                (current.get(a, 0)+current.get(b,0))/2, (current.get(a, 1)+current.get(b,1))/2, (current.get(a, 2)+current.get(b, 2))/2, 1,
                current.get(c, 0), current.get(c, 1), current.get(c, 2), 1
            }), color
        ),
        Triangle(
            std::vector<double>({
                current.get(b, 0), current.get(b, 1), current.get(b, 2), 1,
                (current.get(a, 0)+current.get(b,0))/2, (current.get(a, 1)+current.get(b,1))/2, (current.get(a, 2)+current.get(b, 2))/2, 1,
                current.get(c, 0), current.get(c, 1), current.get(c, 2), 1
            }), color
        )
    );
}

double Triangle::getLongestLen()
{
    return std::max(std::max(getEdgeLen(0, 1), getEdgeLen(0, 2)), getEdgeLen(1, 2));
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
