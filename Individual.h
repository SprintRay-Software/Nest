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
    vector<NestPath> &getPlacement();
    void setPlacement(vector<NestPath> placement);
    vector<int> getRotation();
    void setRotation(vector<int> rotation);
    int compareTo(Individual o);
    bool equals(Individual obj);
    string toString();
    double getFitness();
    void setFitness(double fitness);
    bool operator==(const Individual& ind);
    bool operator < (const Individual& ind);
    static bool compare(Individual ind1,Individual ind2);

public:
    vector<NestPath> placement;
    vector<int> rotation;
    double fitness;
};
