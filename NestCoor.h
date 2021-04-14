#pragma once

class NestCoor
{
public:
	NestCoor();
	NestCoor(double x, double y);
    double getX();
    void setX(double x);
    double getY();
    void setY(double y);

public:
    double x;
    double y;
};
