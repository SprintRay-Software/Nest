#pragma once
#include "Segments.h"

class Placement
{
public:
	Placement(int bid, Segments translate, double rotate, string name);
	Placement();

public:
    int bid;
    Segments translate;
    double rotate;
    string name;
    double coorX;
    double coorY;
    bool isTranslate;
};
