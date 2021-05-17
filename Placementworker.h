#pragma once
#include "string"
#include "map"
#include "NestPath.h"
#include "Config.h"
#include "Vector.h"
#include "clipper.hpp"
#include "Result.h"
#include "Gson.h"

using namespace ClipperLib;

class Placementworker
{
public:
	Placementworker(NestPath binPolygon, Config config, map<string, vector<NestPath>> nfpCache);
    Result placePaths(vector<NestPath> &paths);
    static Path scaleUp2ClipperCoordinates(NestPath polygon);
    static NestPath toNestCoordinates(Path polygon);

public:
    NestPath binPolygon;
    Config config;
    map<string, vector<NestPath>> nfpCache;

private:
    static Gson *gson;

};
