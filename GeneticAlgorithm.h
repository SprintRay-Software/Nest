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
	void Generation();
	vector<Individual> Mate(Individual male, Individual female);
	bool Contains(vector<NestPath> gene, int id);
	Individual RandomWeightedIndividual(Individual *exclude);
	void Init();
	Individual Mutate(Individual individual);
	int RandomAngle(NestPath part);
	vector<NestPath> GetAdam();
	void SetAdam(vector<NestPath> adam);
	NestPath GetBin();
	void SetBin(NestPath bin);
	void CheckAndUpdate(Individual &individual);
	void Shuffle(vector<int> &paramObject);

public:
	vector<NestPath> m_adam;
	NestPath m_bin;
	Bound m_binBounds;
	vector<int> m_angles;
	vector<Individual> *m_population;
	Config m_config;
};
