#pragma once
#include "string"
#include "NestPath.h"

using namespace std;

class Individual
{
public:
	//Individual(Individual individual);
	Individual();
	Individual(vector<NestPath> placement, vector<int> rotation);
	int Size();
	vector<NestPath> &GetPlacement();
	void SetPlacement(vector<NestPath> placement);
	vector<int> GetRotation();
	void SetRotation(vector<int> rotation);
	int CompareTo(Individual o);
	bool Equals(Individual obj);
	string ToString();
	double GetFitness();
	void SetFitness(double fitness);
	bool operator==(const Individual& Ind);
	bool operator < (const Individual& Ind);
    static bool Compare(Individual ind1,Individual ind2);

//    struct LocMinSorter
//    {
//      inline bool operator()(const Individual& locMin1, const Individual& locMin2)
//      {
//        return locMin2.m_fitness > locMin1.m_fitness;
//      }
//    };

public:
	vector<NestPath> m_placement;
	vector<int> m_rotation;
	double m_fitness;
};
