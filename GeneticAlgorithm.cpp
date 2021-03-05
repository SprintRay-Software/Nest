#include <iostream>
#include <random>
#include <chrono>
#include "GeneticAlgorithm.h"
#include "GeometryUtil.h"

struct LocMaxSorter
{
  inline bool operator()(const Individual& locMin1, const Individual& locMin2)
  {
    return locMin2.fitness < locMin1.fitness;
  }
};

GeneticAlgorithm::GeneticAlgorithm(vector<NestPath> adam, NestPath bin, Config config)
{
    this->adam = adam;
    this->bin = bin;
    this->config = config;
    this->binBounds = GeometryUtil::getPolygonBounds(bin);
    this->population = new vector<Individual>();
    init();
}

void GeneticAlgorithm::generation()
{
	vector<Individual> *newpopulation = new vector<Individual>();
    std::sort(population->begin(), population->end(),LocMaxSorter());
    newpopulation->push_back(population->at(0));
    while (newpopulation->size() < config.populationSize)
	{
        Individual male = randomWeightedIndividual(NULL);
        Individual female = randomWeightedIndividual(&male);
        vector<Individual> children = mate(male, female);
        newpopulation->push_back(mutate(children.at(0)));
        if (newpopulation->size() < population->size())
		{
            newpopulation->push_back(mutate(children.at(1)));
		}
	}
    population = newpopulation;
}

vector<Individual> GeneticAlgorithm::mate(Individual male, Individual female)
{
	vector<Individual> children;
    double tempMax = max(rand() % 1000 / 1000.0, 0.1);
    double tempMin = min(tempMax, 0.9);
    int si = male.placement.size() - 1;
    long cutPoint = round(tempMin*si);

	vector<NestPath> gene1;
	vector<int> rot1;
	vector<NestPath> gene2;
	vector<int> rot2;

    NestPath tempNp;
    for (int i = 0; i < cutPoint; i++)
	{
        tempNp = male.placement.at(i);
        gene1.push_back(tempNp);
        rot1.push_back(male.getRotation().at(i));
        tempNp = female.placement.at(i);
        gene2.push_back(tempNp);
        rot2.push_back(female.getRotation().at(i));
	}

    for (int i = 0; i < female.placement.size(); i++)
	{
        if (!contains(gene1, female.placement.at(i).getId()))
		{
            gene1.push_back(female.placement.at(i));
            rot1.push_back(female.rotation.at(i));
		}
	}

    for (int i = 0; i < male.placement.size(); i++)
	{
        if (!contains(gene2, male.placement.at(i).getId()))
		{
            gene2.push_back(male.placement.at(i));
            rot2.push_back(male.rotation.at(i));
		}
	}
	Individual individual1(gene1, rot1);
	Individual individual2(gene2, rot2);

    checkAndUpdate(individual1); checkAndUpdate(individual2);


	children.push_back(individual1); children.push_back(individual2);
	return children;
}

bool GeneticAlgorithm::contains(vector<NestPath> gene, int id)
{
	for (int i = 0; i < gene.size(); i++) 
	{
        if (gene.at(i).getId() == id)
		{
			return true;
		}
	}
	return false;
}

Individual GeneticAlgorithm::randomWeightedIndividual(Individual *exclude)
{
	vector<Individual> pop;
    for (int i = 0; i < population->size(); i++)
	{
        Individual individual = population->at(i);
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
    double tempRand = rand() % 1000 / 1000.0;
	double lower = 0;
	double weight = 1 / pop.size();
	double upper = weight;

	for (int i = 0; i < pop.size(); i++) 
	{
        if (tempRand > lower && tempRand < upper)
		{
			return pop.at(i);
		}
		lower = upper;
		upper += 2 * weight * ((pop.size() - i) / pop.size());
	}
	return pop.at(0);
}

void GeneticAlgorithm::init()
{
    for (int i = 0; i < adam.size(); i++)
	{
        int angle = randomAngle(adam.at(i));
        angles.push_back(angle);
	}
    population->push_back(Individual(adam, angles));    //this individual's fitness =-1
    //std::cout<<"wangjx adam.fitness"<<population->at(0).fitness<<std::endl;
    while (population->size() < config.populationSize)
	{
        //std::cout<<"wangjx population->at(0).fitness"<<population->at(0).fitness<<std::endl;
        Individual mutant = mutate(population->at(0));
        //std::cout<<"wangjx population->at(0).fitness1"<<population->at(0).fitness<<std::endl;
        population->push_back(mutant);
	}
}

Individual GeneticAlgorithm::mutate(Individual individual)
{
	Individual clone(individual);
    for (int i = 0; i < clone.placement.size(); i++)
	{
		double random = rand() % 1000 / 1000.0;
        if (random < 0.05 * config.mutationRate)
		{
			int j = i + 1;
            if (j < clone.placement.size())
			{
                swap(clone.getPlacement()[i], clone.getPlacement()[j]);
			}
		}
		random = rand() % 1000 / 1000.0;
        if (random < 0.05 * config.mutationRate)
		{
            int angle = randomAngle(clone.placement.at(i));
            clone.getRotation()[i] = angle;
            clone.rotation[i] = angle;
		}
	}
    checkAndUpdate(clone);
	return clone;
}


int GeneticAlgorithm::randomAngle(NestPath part)
{
	vector<int> angleList;
    int rotate = max(1, part.getRotation());
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
        Bound rotatedPart = GeometryUtil::rotatePolygon(part, angleList.at(i));
        if (rotatedPart.getWidth() < binBounds.getWidth() && rotatedPart.getHeight() < binBounds.getHeight())
		{
			return angleList.at(i);
		}
	}
	return -1;
}

vector<NestPath> GeneticAlgorithm::getAdam()
{
    return adam;
}

void GeneticAlgorithm::setAdam(vector<NestPath> adam)
{
    this->adam = adam;
}

NestPath GeneticAlgorithm::getBin()
{
    return bin;
}

void GeneticAlgorithm::setBin(NestPath bin)
{
    this->bin = bin;
}

void GeneticAlgorithm::checkAndUpdate(Individual &individual)
{
    for (int i = 0; i < individual.placement.size(); i++)
	{
        int angle = individual.getRotation().at(i);
        NestPath nestPath = individual.getPlacement().at(i);
        Bound rotateBound = GeometryUtil::rotatePolygon(nestPath, angle);
        if (rotateBound.width < binBounds.width && rotateBound.height < binBounds.height)
		{
			continue;
		}
		else 
		{
            int safeAngle = randomAngle(nestPath);
            individual.getRotation()[i] = safeAngle;
		}
	}
}

void GeneticAlgorithm::shuffle(vector<int> &paramObject)
{
    vector<int> tempObject = paramObject;
    for (int i = tempObject.size() - 1; i > 0; i--)
	{
		int pos = rand() % i;
        paramObject[i] = tempObject[pos];
        for (int j = pos; j < tempObject.size() - 1; j++)
		{
            tempObject[j] = tempObject[j + 1];
		}
	}
    paramObject[0] = tempObject[0];
}

