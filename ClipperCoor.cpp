#include "ClipperCoor.h"


ClipperCoor::ClipperCoor() 
{
}

ClipperCoor::ClipperCoor(long x, long y) 
{
    this->x = x;
    this->y = y;
}

long ClipperCoor::getX()
{
    return x;
}

void ClipperCoor::setX(long x)
{
    this->x = x;
}

long ClipperCoor::getY()
{
    return y;
}

void ClipperCoor::setY(long y)
{
    this->y = y;
}
