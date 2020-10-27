#include "ParallelData.h"


ParallelData::ParallelData()
{
    value = new vector<NestPath>();
}

ParallelData::ParallelData(NfpKey key, vector<NestPath> *value)
{
    this->key = key;
    this->value = value;
}

NfpKey ParallelData::getKey()
{
    return key;
}

void ParallelData::setKey(NfpKey key)
{
    this->key = key;
}

vector<NestPath> ParallelData::getValue()
{
    return *value;
}

void ParallelData::setValue(vector<NestPath> *value)
{
    this->value = value;
}

