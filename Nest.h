#pragma once
#include <map>
#include "GeneticAlgorithm.h"";
#include "Individual.h";
#include "vector.h"
#include "Placement.h"
#include "ParallelData.h"
#include "NfpPair.h" 
#include "NfpUtil.h"
#include "Gson.h"
#include "Result.h"


class Nest
{
public:
	Nest(NestPath binPath_para, vector<NestPath> parts, Config config, int count);
	vector<vector<Placement>> StartNest();
	Result LaunchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config);
	static vector<vector<Placement>> ApplyPlacement(Result best, vector<NestPath> tree);
	static vector<int>  CheckIfCanBePlaced(NestPath binPolygon, vector<NestPath> tree);
	void Add(NestPath np);
	NestPath GetBinPath();
	vector<NestPath> GetParts();
	void SetBinPath(NestPath binPath);
	void SetParts(vector<NestPath> parts);
	Config GetConfig();
	void SetConfig(Config config);
	//void sort(vector<NestPath> np);
	//void sort(vector<Individual> Ind);

private:
	NestPath m_binPath;
	vector<NestPath> m_parts;
	Config m_config;
	int m_loopCount;
	GeneticAlgorithm *m_GA = NULL;
	map<string, vector<NestPath>> *m_nfpCache;
	static Gson *m_gson;
	int m_launchcount = 0;
};

