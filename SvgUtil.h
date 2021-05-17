#pragma once
#include <list>
#include "string"
#include "NestPath.h"
#include "Placement.h"
#include "vector.h"

using namespace std;

class SvgUtil
{
public:
    static vector<string> svgGenerator(vector<NestPath> list, vector<vector<Placement>> applied, double binWidth, double binHeight);
    static NestPath getNestPathByBid(int bid, vector<NestPath> list);
};
