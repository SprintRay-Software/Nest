#include "Vector.h"


Vector::Vector(double x, double y, int id, int rotation)
{
    this->x = x;
    this->y = y;
    this->id = id;
    this->rotation = rotation;
}

Vector::Vector(double x, double y, int id, int rotation, Paths nfp)
{
    this->x = x;
    this->y = y;
    this->id = id;
    this->rotation = rotation;
    this->nfp = nfp;
}

Vector::Vector()
{

}


string Vector::toString()
{
    return  "x = " + to_string(x) + " , y = " + to_string(y);
}

