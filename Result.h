#pragma once
#include "Vector.h"
#include "NestPath.h"

using namespace std;

class Result
{
public:
	Result(vector<vector<Vector>> placements, double fitness, vector<NestPath> paths, double area);
	Result();

public:
    vector<vector<Vector>> placements;
    double fitness;
    vector<NestPath> paths;
    double area;
};
