#include "Result.h"
#include "vector.h"


Result::Result(vector<vector<Vector>> placements, double fitness, vector<NestPath> paths, double area)
{
    this->placements = placements;
    this->fitness = fitness;
    this->paths = paths;
    this->area = area;
}

Result::Result()
{
}
