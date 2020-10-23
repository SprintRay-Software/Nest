#include "Result.h"
#include "vector.h"


Result::Result(vector<vector<Vector>> placements, double fitness, vector<NestPath> paths, double area)
{
	m_placements = placements;
	m_fitness = fitness;
	m_paths = paths;
	m_area = area;
}

Result::Result()
{
}
