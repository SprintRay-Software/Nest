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
    vector<vector<Placement>> startNest();
    Result launchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config);
    static vector<vector<Placement>> applyPlacement(Result best, vector<NestPath> tree);
    static vector<int>  checkIfCanBePlaced(NestPath binPolygon, vector<NestPath> tree);
    void Add(NestPath np);
    NestPath getBinPath();
    vector<NestPath> getParts();
    void setBinPath(NestPath binPath);
    void setParts(vector<NestPath> parts);
    Config getConfig();
    void setConfig(Config config);

private:
    NestPath binPath;
    vector<NestPath> parts;
    Config config;
    int loopCount;
    GeneticAlgorithm *GA = NULL;
    map<string, vector<NestPath>> *nfpCache;
    static Gson *gson;
    int launchCount = 0;
};

