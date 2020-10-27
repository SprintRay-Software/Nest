#include "Bound.h"


Bound::Bound(double minX, double minY, double width, double height)
{
    this->minX = minX;
    this->minY = minY;
    this->width = width;
    this->height = height;
}

Bound::Bound()
{
}

double Bound::getMinX()
{
    return minX;
}

void Bound::setMinX(double minX)
{
    this->minX = minX;
}

double Bound::getMinY()
{
    return minY;
}

void Bound::setMinY(double minY)
{
    this->minY = minY;
}

double Bound::getWidth()
{
    return width;
}

void Bound::setWidth(double width)
{
    this->width = width;
}

double Bound::getHeight()
{
    return height;
}

void Bound::setHeight(double height)
{
    this->height = height;
}

string Bound::toString()
{
    return "minX = " + to_string(minX) + " , minY = " + to_string(minY) + " , width = " + to_string(width) + ", height = " + to_string(height);
}

