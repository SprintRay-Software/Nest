#include "Individual.h"


Individual::Individual() 
{
    this->fitness = -1;
    //这是什么操作
    //this->placement;
    //this->rotation;
}

Individual::Individual(vector<NestPath> placement, vector<int> rotation)
{
    this->fitness = -1;
    this->placement = placement;
    this->rotation = rotation;
}

int Individual::Size() 
{
    return placement.size();
}

vector<NestPath>& Individual::getPlacement()
{
    return placement;
}

void Individual::setPlacement(vector<NestPath> placement)
{
    this->placement = placement;
}

vector<int> Individual::getRotation()
{
    return rotation;
}

void Individual::setRotation(vector<int> rotation)
{
    this->rotation = rotation;
}

int Individual::compareTo(Individual o)
{
    return fitness > o.fitness;
}

bool Individual::equals(Individual obj)
{
	Individual individual = (Individual)obj;
    if (placement.size() != individual.Size())
	{
		return false;
	}
    for (int i = 0; i < placement.size(); i++)
	{
        if (!placement.at(i).equals(individual.getPlacement().at(i)))
		{
			return false;
		}
	}
    if (rotation.size() != individual.getRotation().size())
	{
		return false;
	}
    for (int i = 0; i < rotation.size(); i++)
	{
        if (rotation.at(i) != individual.getRotation().at(i))
		{
			return false;
		}
	}
	return true;
}

string Individual::toString()
{
	string res = "";
	int count = 0;
    for (int i = 0; i < placement.size(); i++)
	{
		res += "NestPath " + to_string(count) + "\n";
		count++;
        res += placement.at(i).toString() + "\n";
	}
	res += "rotation \n";
	//for (int r : rotation) 
	int r;
    for (int i = 0; i < rotation.size(); i++)
	{
        r = rotation.at(i);
		res += r + " ";
	}
	res += "\n";

	return res;
}

double Individual::getFitness()
{
    return fitness;
}

void Individual::setFitness(double fitness)
{
    this->fitness = fitness;
}

bool Individual::operator==(const Individual& Ind)
{
    if (fitness == Ind.fitness)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Individual::operator < (const Individual& Ind)
{
    return compareTo(Ind) < 0;
}

bool Individual::compare(Individual ind1,Individual ind2)
{
    return 0;
}

