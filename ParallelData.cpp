#include "ParallelData.h"


ParallelData::ParallelData()
{
	m_value = new vector<NestPath>();
}

ParallelData::ParallelData(NfpKey key, vector<NestPath> *value)
{
	m_key = key;
	m_value = value;
}

NfpKey ParallelData::GetKey()
{
	return m_key;
}

void ParallelData::SetKey(NfpKey key)
{
	m_key = key;
}

vector<NestPath> ParallelData::GetValue()
{
	return *m_value;
}

void ParallelData::SetValue(vector<NestPath> *value)
{
	m_value = value;
}

