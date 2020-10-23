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
	Result PlacePaths(vector<NestPath> &paths);
	static Path ScaleUp2ClipperCoordinates(NestPath polygon);
	static NestPath ToNestCoordinates(Path polygon);

public:
	NestPath m_binPolygon;
	Config m_config;
	map<string, vector<NestPath>> m_nfpCache;

private:
	static Gson *m_gson;

};
