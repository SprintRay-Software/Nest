#pragma once
#include "string"

using namespace std;

class Bound
{
public:
    Bound(double minX, double minY, double width, double height);
	Bound();
    double getMinX();
    void setMinX(double minX);
    double getMinY();
    void setMinY(double minY);
    double getWidth();
    void setWidth(double width);
    double getHeight();
    void setHeight(double height);
    string toString();

public:
    double minX;
    double minY;
    double width;
    double height;
};
