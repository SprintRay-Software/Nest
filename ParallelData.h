#pragma once
#include "NfpKey.h"
#include "NestPath.h"
#include "vector.h"


class ParallelData
{
public:
	ParallelData();
	ParallelData(NfpKey key, vector<NestPath> *value);
	NfpKey GetKey();
	void SetKey(NfpKey key);
	vector<NestPath> GetValue();
	void SetValue(vector<NestPath> *value);

public:
	NfpKey m_key;
	vector<NestPath> *m_value;
};