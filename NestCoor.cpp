#include "NestCoor.h"


NestCoor::NestCoor()
{
}

NestCoor::NestCoor(double x, double y)
{
    this->x = x;
    this->y = y;
}

double NestCoor::getX()
{
    return x;
}

void NestCoor::setX(double x)
{
    this->x = x;
}

double NestCoor::getY()
{
    return y;
}

void NestCoor::setY(double y)
{
    this->y = y;
}
