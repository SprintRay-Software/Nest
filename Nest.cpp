#include "GeneticAlgorithm.h"";
#include "Individual.h";
#include "Nest.h"
#include "CommonUtil.h"
#include "GeometryUtil.h"


struct LocMinSorter
{
  inline bool operator()(const NestPath& locMin1, const NestPath& locMin2)
  {
    return locMin2.area > locMin1.area;
  }
};

Gson* Nest::gson = new Gson();


Nest::Nest(NestPath binPath, vector<NestPath> parts, Config config, int count)
{
    this->binPath = binPath;
    this->parts = parts;
    this->config = config;
    this->loopCount = count;
    this->nfpCache = new map<string, vector<NestPath>>();
}


vector<vector<Placement>> Nest::startNest()
{



    vector<NestPath> tree = CommonUtil::buildTree(parts, config.curveTolerance);

    CommonUtil::offsetTree(tree, 0.5 * config.spacing);
    binPath.config = &config;
	NestPath nestPath;
    for (int i = 0; i < parts.size(); i++)
	{
        nestPath = parts.at(i);
        nestPath.config = &config;
	}
    NestPath binPolygon = (NestPath::cleanNestPath(binPath));
    Bound binBound = GeometryUtil::getPolygonBounds(binPolygon);
    if (config.spacing > 0)
	{
        vector<NestPath> offsetBin = CommonUtil::polygonOffset(binPolygon, -0.5 * config.spacing);
		if (offsetBin.size() == 1) 
		{
			binPolygon = offsetBin.at(0);
		}
	}
    binPolygon.setId(-1);

    vector<int> integers = checkIfCanBePlaced(binPolygon, tree);
	vector<NestPath> safeTree;
	int j;
	for (int i = 0; i < integers.size(); i++)
	{
		j = integers.at(i);
		safeTree.push_back(tree.at(j));
	}
	tree = safeTree;

    double xBinMax = binPolygon.Get(0).x;
    double xBinMin = binPolygon.Get(0).x;
    double yBinMax = binPolygon.Get(0).y;
    double yBinMin = binPolygon.Get(0).y;

	for (int i = 1; i < binPolygon.Size(); i++) 
	{
        if (binPolygon.Get(i).x > xBinMax)
		{
            xBinMax = binPolygon.Get(i).x;
		}
        else if (binPolygon.Get(i).x < xBinMin)
		{
            xBinMin = binPolygon.Get(i).x;
		}

        if (binPolygon.Get(i).y > yBinMax)
		{
            yBinMax = binPolygon.Get(i).y;
		}
        else if (binPolygon.Get(i).y < yBinMin)
		{
            yBinMin = binPolygon.Get(i).y;
		}
	}
	for (int i = 0; i < binPolygon.Size(); i++) 
	{
        binPolygon.Get(i).x -= xBinMin;
        binPolygon.Get(i).y -= yBinMin;
        double a = binPolygon.Get(i).x;
	}


    double binPolygonWidth = xBinMax - xBinMin;
    double binPolygonHeight = yBinMax - yBinMin;

    if (GeometryUtil::polygonArea(binPolygon) > 0)
	{
        binPolygon.reverse();
	}

	for (int i = 0; i < tree.size(); i++) 
	{
		Segments start = tree.at(i).Get(0);
		Segments end = tree.at(i).Get(tree.at(i).Size() - 1);
        if (start == end || GeometryUtil::almostEqual(start.x, end.x) && GeometryUtil::almostEqual(start.y, end.y))
		{
			tree.at(i).Pop();
		}
        if (GeometryUtil::polygonArea(tree.at(i)) > 0)
		{
            tree.at(i).reverse();
		}
	}

    launchCount = 0;
	Result best;
    for (int i = 0; i < loopCount; i++)
	{

        Result result = launchWorkers(tree, binPolygon, config);

        std::cout << "fitness: " << result.fitness << endl;

		if (i == 0) 
		{
			best = result;
		}
		else 
		{
            if (best.fitness > result.fitness)
			{
				best = result;
			}
		}
	}
    double sumArea = 0;
    double totalArea = 0;
    for (int i = 0; i < best.placements.size(); i++)
	{
        totalArea += abs(GeometryUtil::polygonArea(binPolygon));
        for (int j = 0; j < best.placements.at(i).size(); j++)
		{
            sumArea += abs(GeometryUtil::polygonArea(tree.at(best.placements.at(i).at(j).id)));
		}
	}
    double rate = (sumArea / totalArea) * 100;
    vector<vector<Placement>> appliedPlacement = applyPlacement(best, tree);
	return appliedPlacement;
}


Result Nest::launchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config)
{
    launchCount++;
    if (GA == NULL)
	{
		vector<NestPath> adam;
		NestPath nestPath;
		for (int i = 0; i < tree.size(); i++)
		{
			nestPath = tree.at(i);
			NestPath *clone = new NestPath(nestPath);
			adam.push_back(*clone);
            adam.at(i).area = GeometryUtil::polygonArea(nestPath);
		}
        std::sort(adam.begin(), adam.end(), LocMinSorter());
        GA = new GeneticAlgorithm(adam, binPolygon, config);
	}

	Individual *individual = NULL;
    for (int i = 0; i < GA->population->size(); i++)
	{
        if (GA->population->at(i).getFitness() < 0)
		{
            individual = &(GA->population->at(i));
			break;
		}
	}
    if (launchCount > 1 && individual == NULL)
	{
        GA->generation();
        *individual = GA->population->at(1);
	}

    vector<NestPath> placeList = individual->getPlacement();
    vector<int> rotations = individual->getRotation();

	vector<int> ids;
    for (int i = 0; i < placeList.size(); i++)
	{
        ids.push_back(placeList.at(i).getId());
        placeList.at(i).setRotation(rotations.at(i));
	}
	vector<NfpPair> nfpPairs;
	NfpKey key;

    for (int i = 0; i < placeList.size(); i++)
	{
        NestPath part = placeList.at(i);
        key = NfpKey(binPolygon.getId(), part.getId(), true, 0, part.getRotation());
		nfpPairs.push_back(NfpPair(binPolygon, part, key));
		for (int j = 0; j < i; j++) 
		{
            NestPath placed = placeList.at(j);
            NfpKey keyed(placed.getId(), part.getId(), false, rotations.at(j), rotations.at(i));
			nfpPairs.push_back(NfpPair(placed, part, keyed));
		}
	}


	vector<ParallelData> generatedNfp;
	for (int i = 0; i < nfpPairs.size(); i++)
	{
		NfpPair nfpPair = nfpPairs.at(i);
		ParallelData *data = NfpUtil::NfpGenerator(nfpPair, config);
		generatedNfp.push_back(*data);
	}
	for (int i = 0; i < generatedNfp.size(); i++)
	{
		ParallelData Nfp = generatedNfp.at(i);
        string tkey = gson->toJson(Nfp.getKey());
        nfpCache->insert(make_pair(tkey, *Nfp.value));
	}


    Placementworker worker(binPolygon, config, *nfpCache);
	vector<NestPath> placeListSlice;

    for (int i = 0; i < placeList.size(); i++)
	{
        placeListSlice.push_back(placeList.at(i));
	}
	vector<vector<NestPath>> data;
	data.push_back(placeListSlice);
	vector<Result> placements;
	for (int i = 0; i < data.size(); i++) 
	{
        Result result = worker.placePaths((data.at(i)));
        std::cout << "result.m_fitness: " << result.fitness << endl;
		placements.push_back(result);
	}
	if (placements.size() == 0) 
	{
		Result bestResult1;
		return bestResult1;
	}
    individual->fitness = placements.at(0).fitness;
	Result bestResult = placements.at(0);
	for (int i = 1; i < placements.size(); i++) 
	{
        if (placements.at(i).fitness < bestResult.fitness)
		{
			bestResult = placements.at(i);
		}
	}
	return bestResult;
}

vector<vector<Placement>> Nest::applyPlacement(Result best, vector<NestPath> tree)
{
	vector<vector<Placement>> applyPlacement;
    for (int i = 0; i < best.placements.size(); i++)
	{
		vector<Placement> binTranslate;
        for (int j = 0; j < best.placements.at(i).size(); j++)
		{
            Vector v = best.placements.at(i).at(j);
            NestPath nestPath = tree.at(v.id);
			NestPath child;
            for (int i = 0; i < nestPath.getChildren()->size(); i++)
			{
                child = nestPath.getChildren()->at(i);
                Placement chPlacement(child.bid, Segments(v.x, v.y), v.rotation, child.name);
				binTranslate.push_back(chPlacement);
			}
            Placement placement(nestPath.bid, Segments(v.x, v.y), v.rotation, nestPath.name);
			binTranslate.push_back(placement);
		}
		applyPlacement.push_back(binTranslate);
	}
	return applyPlacement;
}


vector<int> Nest::checkIfCanBePlaced(NestPath binPolygon, vector<NestPath> tree)
{
    vector<int> canBePlacdPolygonIndex;
    Bound binBound = GeometryUtil::getPolygonBounds(binPolygon);
	for (int i = 0; i < tree.size(); i++) 
	{
		NestPath nestPath = tree.at(i);
        if (nestPath.getRotation() == 0)
		{
            Bound bound = GeometryUtil::getPolygonBounds(nestPath);
            if (bound.width < binBound.width && bound.height < binBound.height)
			{
                canBePlacdPolygonIndex.push_back(i);
				continue;
			}
		}
		else {
            for (int j = 0; j < nestPath.getRotation(); j++)
			{
                Bound rotatedBound = GeometryUtil::rotatePolygon(nestPath, (360 / nestPath.getRotation()) * j);
                if (rotatedBound.width < binBound.width && rotatedBound.height < binBound.height)
				{
                    canBePlacdPolygonIndex.push_back(i);
					break;
				}
			}
		}
	}
    return canBePlacdPolygonIndex;
}



void Nest::Add(NestPath np) 
{
    parts.push_back(np);
}

NestPath Nest::getBinPath()
{
    return binPath;
}

vector<NestPath> Nest::getParts()
{
    return parts;
}

void Nest::setBinPath(NestPath binPath)
{
    this->binPath = binPath;
}

void Nest::setParts(vector<NestPath> parts)
{
    this->parts = parts;
}

Config Nest::getConfig()
{
    return config;
}

void Nest::setConfig(Config config)
{
    this->config = config;
}


