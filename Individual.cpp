#include "Individual.h"


Individual::Individual() 
{
	m_fitness = -1;
	m_placement;
	m_rotation;
}

Individual::Individual(vector<NestPath> placement, vector<int> rotation)
{
	m_fitness = -1;
	m_placement = placement;
	m_rotation = rotation;
}

int Individual::Size() 
{
	return m_placement.size();
}

vector<NestPath>& Individual::GetPlacement()
{
	return m_placement;
}

void Individual::SetPlacement(vector<NestPath> placement)
{
	m_placement = placement;
}

vector<int> Individual::GetRotation()
{
	return m_rotation;
}

void Individual::SetRotation(vector<int> rotation)
{
	m_rotation = rotation;
}

int Individual::CompareTo(Individual o)
{
	return m_fitness > o.m_fitness;
}

bool Individual::Equals(Individual obj)
{
	Individual individual = (Individual)obj;
	if (m_placement.size() != individual.Size())
	{
		return false;
	}
	for (int i = 0; i < m_placement.size(); i++)
	{
		if (!m_placement.at(i).Equals(individual.GetPlacement().at(i)))
		{
			return false;
		}
	}
	if (m_rotation.size() != individual.GetRotation().size())
	{
		return false;
	}
	for (int i = 0; i < m_rotation.size(); i++)
	{
		if (m_rotation.at(i) != individual.GetRotation().at(i))
		{
			return false;
		}
	}
	return true;
}

string Individual::ToString() 
{
	string res = "";
	int count = 0;
	for (int i = 0; i < m_placement.size(); i++)
	{
		res += "NestPath " + to_string(count) + "\n";
		count++;
		res += m_placement.at(i).ToString() + "\n";
	}
	res += "rotation \n";
	//for (int r : rotation) 
	int r;
	for (int i = 0; i < m_rotation.size(); i++)
	{
		r = m_rotation.at(i);
		res += r + " ";
	}
	res += "\n";

	return res;
}

double Individual::GetFitness() 
{
	return m_fitness;
}

void Individual::SetFitness(double fitness)
{
	m_fitness = fitness;
}

bool Individual::operator==(const Individual& Ind)
{
	if (m_fitness == Ind.m_fitness)
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
	return CompareTo(Ind) < 0;
}

bool Individual::Compare(Individual ind1,Individual ind2)
{
    return 0;
}

