#include <iostream>
#include <random>
#include <chrono>
#include "GeneticAlgorithm.h"
#include "GeometryUtil.h"

struct LocMaxSorter
{
  inline bool operator()(const Individual& locMin1, const Individual& locMin2)
  {
    return locMin2.m_fitness < locMin1.m_fitness;
  }
};

GeneticAlgorithm::GeneticAlgorithm(vector<NestPath> adam, NestPath bin, Config config)
{
	m_adam = adam;
	m_bin = bin;
	m_config = config;
	m_binBounds = GeometryUtil::GetPolygonBounds(bin);
	m_population = new vector<Individual>();
	Init();
}

void GeneticAlgorithm::Generation()
{
	vector<Individual> *newpopulation = new vector<Individual>();
    std::sort(m_population->begin(), m_population->end(),LocMaxSorter());
	newpopulation->push_back(m_population->at(0));
	while (newpopulation->size() < m_config.m_population_size)
	{
		Individual male = RandomWeightedIndividual(NULL);
		Individual female = RandomWeightedIndividual(&male);
		vector<Individual> children = Mate(male, female);
		newpopulation->push_back(Mutate(children.at(0)));
		if (newpopulation->size() < m_population->size())
		{
			newpopulation->push_back(Mutate(children.at(1)));
		}
	}
	m_population = newpopulation;
}

vector<Individual> GeneticAlgorithm::Mate(Individual male, Individual female)
{
	vector<Individual> children;
	double max_temp = max(rand() % 1000 / 1000.0, 0.1);
	double min_temp = min(max_temp, 0.9);
	int si = male.m_placement.size() - 1;
	long cutpoint = round(min_temp*si);

	vector<NestPath> gene1;
	vector<int> rot1;
	vector<NestPath> gene2;
	vector<int> rot2;

	NestPath np_temp;
	for (int i = 0; i < cutpoint; i++) 
	{
		np_temp = male.m_placement.at(i);
		gene1.push_back(np_temp);
		rot1.push_back(male.GetRotation().at(i));
		np_temp = female.m_placement.at(i);
		gene2.push_back(np_temp);
		rot2.push_back(female.GetRotation().at(i));
	}

	for (int i = 0; i < female.m_placement.size(); i++)
	{
		if (!Contains(gene1, female.m_placement.at(i).GetId()))
		{
			gene1.push_back(female.m_placement.at(i));
			rot1.push_back(female.m_rotation.at(i));
		}
	}

	for (int i = 0; i < male.m_placement.size(); i++)
	{
		if (!Contains(gene2, male.m_placement.at(i).GetId()))
		{
			gene2.push_back(male.m_placement.at(i));
			rot2.push_back(male.m_rotation.at(i));
		}
	}
	Individual individual1(gene1, rot1);
	Individual individual2(gene2, rot2);

	CheckAndUpdate(individual1); CheckAndUpdate(individual2);


	children.push_back(individual1); children.push_back(individual2);
	return children;
}

bool GeneticAlgorithm::Contains(vector<NestPath> gene, int id)
{
	for (int i = 0; i < gene.size(); i++) 
	{
		if (gene.at(i).GetId() == id) 
		{
			return true;
		}
	}
	return false;
}

Individual GeneticAlgorithm::RandomWeightedIndividual(Individual *exclude) 
{
	vector<Individual> pop;
	for (int i = 0; i < m_population->size(); i++)
	{
		Individual individual = m_population->at(i);
		Individual clone(individual);
		pop.push_back(clone);
	}
	if (exclude != NULL) 
	{
		for (auto iter = pop.begin(); iter != pop.end();)
		{
			if (*iter == *exclude)
			{
				iter = pop.erase(iter);
				break;
			}
			else
			{
				iter++;
			}
		}
	}
	double rand_temp = rand() % 1000 / 1000.0;
	double lower = 0;
	double weight = 1 / pop.size();
	double upper = weight;

	for (int i = 0; i < pop.size(); i++) 
	{
		if (rand_temp > lower && rand_temp < upper)
		{
			return pop.at(i);
		}
		lower = upper;
		upper += 2 * weight * ((pop.size() - i) / pop.size());
	}
	return pop.at(0);
}

void GeneticAlgorithm::Init() 
{
	for (int i = 0; i < m_adam.size(); i++)
	{
		int angle = RandomAngle(m_adam.at(i));
		m_angles.push_back(angle);
	}
	m_population->push_back(Individual(m_adam, m_angles));
	while (m_population->size() < m_config.m_population_size)
	{
		Individual mutant = Mutate(m_population->at(0));
		m_population->push_back(mutant);
	}
}

Individual GeneticAlgorithm::Mutate(Individual individual) 
{
	Individual clone(individual);
	for (int i = 0; i < clone.m_placement.size(); i++)
	{
		double random = rand() % 1000 / 1000.0;
		if (random < 0.05 * m_config.m_mutation_rate)
		{
			int j = i + 1;
			if (j < clone.m_placement.size())
			{
				swap(clone.GetPlacement()[i], clone.GetPlacement()[j]);
			}
		}
		random = rand() % 1000 / 1000.0;
		if (random < 0.05 * m_config.m_mutation_rate)
		{
			int angle = RandomAngle(clone.m_placement.at(i));
			clone.GetRotation()[i] = angle;
			clone.m_rotation[i] = angle;
		}
	}
	CheckAndUpdate(clone);
	return clone;
}


int GeneticAlgorithm::RandomAngle(NestPath part)
{
	vector<int> angleList;
	int rotate = max(1, part.GetRotation());
	if (rotate == 0) 
	{
		angleList.push_back(0);
	}
	else 
	{
		for (int i = 0; i < rotate; i++) 
		{
			angleList.push_back((360 / rotate) * i);
		}
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(angleList.begin(), angleList.end(), std::default_random_engine(seed));
	for (int i = 0; i < angleList.size(); i++) 
	{
		Bound rotatedPart = GeometryUtil::RotatePolygon(part, angleList.at(i));
		if (rotatedPart.GetWidth() < m_binBounds.GetWidth() && rotatedPart.GetHeight() < m_binBounds.GetHeight())
		{
			return angleList.at(i);
		}
	}
	return -1;
}

vector<NestPath> GeneticAlgorithm::GetAdam()
{
	return m_adam;
}

void GeneticAlgorithm::SetAdam(vector<NestPath> adam)
{
	m_adam = adam;
}

NestPath GeneticAlgorithm::GetBin() 
{
	return m_bin;
}

void GeneticAlgorithm::SetBin(NestPath bin) 
{
	m_bin = bin;
}

void GeneticAlgorithm::CheckAndUpdate(Individual &individual) 
{
	for (int i = 0; i < individual.m_placement.size(); i++)
	{
		int angle = individual.GetRotation().at(i);
		NestPath nestPath = individual.GetPlacement().at(i);
		Bound rotateBound = GeometryUtil::RotatePolygon(nestPath, angle);
		if (rotateBound.m_width < m_binBounds.m_width && rotateBound.m_height < m_binBounds.m_height)
		{
			continue;
		}
		else 
		{
			int safeAngle = RandomAngle(nestPath);
			individual.GetRotation()[i] = safeAngle;
		}
	}
}

void GeneticAlgorithm::Shuffle(vector<int> &paramObject)
{
	vector<int> temp_Object = paramObject;
	for (int i = temp_Object.size() - 1; i > 0; i--)
	{
		int pos = rand() % i;
		paramObject[i] = temp_Object[pos];
		for (int j = pos; j < temp_Object.size() - 1; j++)
		{
			temp_Object[j] = temp_Object[j + 1];
		}
	}
	paramObject[0] = temp_Object[0];
}

