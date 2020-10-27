#pragma once
#include <list>  
#include "NestPath.h"
#include "Individual.h"
#include "Bound.h"

using namespace std;

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(vector<NestPath> adam, NestPath bin, Config config);
    void generation();
    vector<Individual> mate(Individual male, Individual female);
    bool contains(vector<NestPath> gene, int id);
    Individual randomWeightedIndividual(Individual *exclude);
    void init();
    Individual mutate(Individual individual);
    int randomAngle(NestPath part);
    vector<NestPath> getAdam();
    void setAdam(vector<NestPath> adam);
    NestPath getBin();
    void setBin(NestPath bin);
    void checkAndUpdate(Individual &individual);
    void shuffle(vector<int> &paramObject);

public:
    vector<NestPath> adam;
    NestPath bin;
    Bound binBounds;
    vector<int> angles;
    vector<Individual> *population;
    Config config;
};
