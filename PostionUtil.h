#pragma once
#include <list> 
#include "NestPath.h"

using namespace std;

class PostionUtil
{
public:
    static list<NestPath> positionTranslate4Path(double x, double y, list<NestPath> paths);

};
