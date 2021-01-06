#include "GeneticAlgorithm.h"";
#include "Individual.h";
#include "Nest.h"
#include "CommonUtil.h"
#include "GeometryUtil.h"
#include "qDebug"


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
    qDebug() << "loopCount:" << loopCount <<endl;
    for (int i = 0; i < loopCount; i++)
	{
        qDebug() << "i:" << i <<endl;
        Result result = launchWorkers(tree, binPolygon, config);

        qDebug() << "return bestResult1" << endl;
		if (i == 0) 
		{
            qDebug() << "return bestResult2" << endl;
			best = result;
            qDebug() << "return bestResult11" << endl;
		}
		else 
		{
            qDebug() << "return bestResult3" << endl;
            if (best.fitness > result.fitness)
			{
                qDebug() << "return bestResult4" << endl;
				best = result;
                qDebug() << "return bestResult33" << endl;
			}
            qDebug() << "return bestResult333" << endl;
		}
	}
    qDebug() << "return bestResult5" << endl;
    double sumArea = 0;
    double totalArea = 0;
    for (int i = 0; i < best.placements.size(); i++)
	{
        qDebug() << "return bestResult6" << endl;
        totalArea += abs(GeometryUtil::polygonArea(binPolygon));
        for (int j = 0; j < best.placements.at(i).size(); j++)
		{
            qDebug() << "return bestResult7" << endl;
            sumArea += abs(GeometryUtil::polygonArea(tree.at(best.placements.at(i).at(j).id)));
		}
	}
    double rate = (sumArea / totalArea) * 100;
    qDebug() << "applyPlacement" << endl;
    vector<vector<Placement>> appliedPlacement = applyPlacement(best, tree);
    qDebug() << "startNest" << endl;
	return appliedPlacement;
}


Result Nest::launchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config)
{
    qDebug() << "launchWorkers" << endl;
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
    qDebug() << "individual" << endl;
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

    qDebug() << "nfpPairs" << endl;
    for (int i = 0; i < placeList.size(); i++)
	{
        //qDebug() << "placeList[i].id" << placeList[i].getId()<< endl;
        //qDebug() << "placeList[i].rotation" << placeList[i].getRotation()<< endl;
        NestPath part = placeList.at(i);
        key = NfpKey(binPolygon.getId(), part.getId(), true, 0, part.getRotation());
        NfpPair nfpPair1(binPolygon, part, key);
        //qDebug() << "NfpKey keyed0:" << nfpPair1.getA().getId()<<","<<nfpPair1.getB().getId()<<","<<nfpPair1.getA().getRotation()<<","<<nfpPair1.getB().getRotation()<< endl;
		nfpPairs.push_back(NfpPair(binPolygon, part, key));
		for (int j = 0; j < i; j++) 
		{
            NestPath placed = placeList.at(j);            
            //qDebug() << "NfpKey keyed1:" << placed.getId()<<","<<part.getId()<<","<<rotations.at(j)<<","<<rotations.at(i)<< endl;
            //qDebug() << "NfpKey keyed2:" << placed.getId()<<","<<part.getId()<<","<<placed.getRotation()<<","<<part.getRotation()<< endl;
            NfpKey keyed(placed.getId(), part.getId(), false, rotations.at(j), rotations.at(i));
            NfpPair nfpPair(placed, part, keyed);
            //qDebug() << "NfpKey keyed3:" << nfpPair.getA().getId()<<","<<nfpPair.getB().getId()<<","<<nfpPair.getA().getRotation()<<","<<nfpPair.getB().getRotation()<< endl;
            nfpPairs.push_back(NfpPair(placed, part, keyed));
		}
	}

//    for (int i = 0; i < nfpPairs.size(); i++)
//    {
//       qDebug() << "NfpKey keyed4:" << nfpPairs.at(i).getA().getId()<<","<<nfpPairs.at(i).getB().getId()<<","<<nfpPairs.at(i).getA().getRotation()<<","<<nfpPairs.at(i).getB().getRotation()<< endl;
//    }

    qDebug() << "generatedNfp" << endl;
	vector<ParallelData> generatedNfp;
	for (int i = 0; i < nfpPairs.size(); i++)
	{
		NfpPair nfpPair = nfpPairs.at(i);
//        qDebug() << "NfpGenerator" << endl;
//        qDebug() << "NfpPairA:" << nfpPair.getA().getId()<<","<<nfpPair.getA().getRotation();
//        qDebug() << "NfpPairB:" << nfpPair.getB().getId()<<","<<nfpPair.getB().getRotation();
//        qDebug() << "NfpPairKEY:" << nfpPair.getKey().getA()<<","<<nfpPair.getKey().getB();
//        qDebug() << "NfpPairKEY:" << nfpPair.getKey().getARotation()<<","<<nfpPair.getKey().getBRotation();
//        qDebug() << "NfpPairKEY:" << nfpPair.getKey().inside;
//        qDebug() << "NfpGenerator before" << endl;
		ParallelData *data = NfpUtil::NfpGenerator(nfpPair, config);
        if(data->value == NULL)
        {
            NestPath b = nfpPair.getB();
            b.setRotation(b.getRotation() + 90);
            nfpPair.setB(b);
            NfpKey keyed1(nfpPair.getA().getId(), nfpPair.getB().getId(), nfpPair.getKey().isInside(), nfpPair.getA().getRotation(), nfpPair.getB().getRotation());
            nfpPair = NfpPair(nfpPair.getA(), nfpPair.getB(), keyed1);
            data = NfpUtil::NfpGenerator(nfpPair, config);
            //qDebug() << "data.key:" << data->getKey().getBRotation();
        }
        qDebug() << "NfpGenerator after" << endl;
		generatedNfp.push_back(*data);
	}
    qDebug() << "generatedNfp.size" <<generatedNfp.size()<< endl;
	for (int i = 0; i < generatedNfp.size(); i++)
	{
        //qDebug() << "toJson before" << endl;
		ParallelData Nfp = generatedNfp.at(i);
        //qDebug() << "*Nfp.value.size:" <<(*(Nfp.value)).size()<<endl;
        //for(int i =0 ;i<(*(Nfp.value)).size();i++)
//        {
//            qDebug() << "*Nfp.value.id1:" << (*(Nfp.value))[0].getId() <<endl;
//            qDebug() << "*Nfp.value.rotation1:" << (*(Nfp.value))[0].getRotation() <<endl;
//            qDebug() << "*Nfp.value.id2:" << (*(Nfp.value))[1].getId() <<endl;
//            qDebug() << "*Nfp.value.rotation2:" << (*(Nfp.value))[1].getRotation() <<endl;
//            qDebug() << "*Nfp.value.id3:" << (*(Nfp.value))[2].getId() <<endl;
//            qDebug() << "*Nfp.value.rotation3:" << (*(Nfp.value))[2].getRotation() <<endl;
//        }
        string tkey = gson->toJson(Nfp.getKey());
        qDebug() << "tkey:" << QString::fromStdString(tkey) <<endl;
        if(Nfp.value == NULL)
        {
            qDebug() << "point is null" << endl;
        }
        try {
                nfpCache->insert(make_pair(tkey, *Nfp.value));
        } catch (...) {
            qDebug() << "insert err" << endl;
        }
        //qDebug() << "toJson after" << endl;
	}

    qDebug() << "Placementworker" << endl;
    Placementworker worker(binPolygon, config, *nfpCache);
	vector<NestPath> placeListSlice;

    for (int i = 0; i < placeList.size(); i++)
	{
        placeListSlice.push_back(placeList.at(i));
	}
	vector<vector<NestPath>> data;
	data.push_back(placeListSlice);
	vector<Result> placements;
    qDebug() << "data.size: " << data.size() << endl;
	for (int i = 0; i < data.size(); i++) 
	{
        Result result = worker.placePaths((data.at(i)));
        qDebug() << "result.m_fitness: " << result.fitness << endl;
		placements.push_back(result);
	}
	if (placements.size() == 0) 
	{
		Result bestResult1;
		return bestResult1;
	}
    qDebug() << "individual->fitness" << endl;
    individual->fitness = placements.at(0).fitness;
	Result bestResult = placements.at(0);
	for (int i = 1; i < placements.size(); i++) 
	{
        if (placements.at(i).fitness < bestResult.fitness)
		{
			bestResult = placements.at(i);
		}
	}
    qDebug() << "return bestResult" << endl;
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


