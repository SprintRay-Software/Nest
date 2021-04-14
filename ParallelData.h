#pragma once
#include "NfpKey.h"
#include "NestPath.h"
#include "vector.h"


class ParallelData
{
public:
	ParallelData();
	ParallelData(NfpKey key, vector<NestPath> *value);
    NfpKey getKey();
    void setKey(NfpKey key);
    vector<NestPath> getValue();
    void setValue(vector<NestPath> *value);

public:
    NfpKey key;
    vector<NestPath> *value;
};
