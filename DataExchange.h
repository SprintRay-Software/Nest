#pragma once
#include "string"
#include "Segments.h"

using namespace std;


class DataExchange
{
public:
	DataExchange(Segments p1, Segments p2, double rx, double ry, double angle, double largearc, int sweep, bool flag);
	DataExchange(Segments center, double rx, double ry, double theta, double extent, double angle, bool flag);
    string toString();

public:
    Segments p1;
    Segments p2;
    Segments center;
    double rx;
    double ry;
    double theta;
    double extent;
    double angle;
    double largearc;
    int sweep;
    bool flag;
};
