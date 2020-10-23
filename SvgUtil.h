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
	static vector<string> SvgGenerator(vector<NestPath> list, vector<vector<Placement>> applied, double binwidth, double binHeight);
	static NestPath GetNestPathByBid(int bid, vector<NestPath> list);
};