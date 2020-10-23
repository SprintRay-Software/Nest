#include "GeneticAlgorithm.h"";
#include "Individual.h";
#include "Nest.h"
#include "CommonUtil.h"
#include "GeometryUtil.h"


struct LocMinSorter
{
  inline bool operator()(const NestPath& locMin1, const NestPath& locMin2)
  {
    return locMin2.m_area > locMin1.m_area;
  }
};

Gson* Nest::m_gson = new Gson();


Nest::Nest(NestPath binPath, vector<NestPath> parts, Config config, int count)
{
	m_binPath = binPath;
	m_parts = parts;
	m_config = config;
	m_loopCount = count;
	m_nfpCache = new map<string, vector<NestPath>>();
}


vector<vector<Placement>> Nest::StartNest()
{



	vector<NestPath> tree = CommonUtil::BuildTree(m_parts, m_config.m_curve_tolerance);

	CommonUtil::OffsetTree(tree, 0.5 * m_config.m_spacing);
	m_binPath.m_config = &m_config;
	NestPath nestPath;
	for (int i = 0; i < m_parts.size(); i++)
	{
		nestPath = m_parts.at(i);
		nestPath.m_config = &m_config;
	}
	NestPath binPolygon = (NestPath::CleanNestPath(m_binPath));
	Bound binBound = GeometryUtil::GetPolygonBounds(binPolygon);
	if (m_config.m_spacing > 0)
	{
		vector<NestPath> offsetBin = CommonUtil::PolygonOffset(binPolygon, -0.5 * m_config.m_spacing);
		if (offsetBin.size() == 1) 
		{
			binPolygon = offsetBin.at(0);
		}
	}
	binPolygon.SetId(-1);

	vector<int> integers = CheckIfCanBePlaced(binPolygon, tree);
	vector<NestPath> safeTree;
	int j;
	for (int i = 0; i < integers.size(); i++)
	{
		j = integers.at(i);
		safeTree.push_back(tree.at(j));
	}
	tree = safeTree;

	double xbinmax = binPolygon.Get(0).m_x;
	double xbinmin = binPolygon.Get(0).m_x;
	double ybinmax = binPolygon.Get(0).m_y;
	double ybinmin = binPolygon.Get(0).m_y;

	for (int i = 1; i < binPolygon.Size(); i++) 
	{
		if (binPolygon.Get(i).m_x > xbinmax)
		{
			xbinmax = binPolygon.Get(i).m_x;
		}
		else if (binPolygon.Get(i).m_x < xbinmin)
		{
			xbinmin = binPolygon.Get(i).m_x;
		}

		if (binPolygon.Get(i).m_y > ybinmax)
		{
			ybinmax = binPolygon.Get(i).m_y;
		}
		else if (binPolygon.Get(i).m_y < ybinmin)
		{
			ybinmin = binPolygon.Get(i).m_y;
		}
	}
	for (int i = 0; i < binPolygon.Size(); i++) 
	{
		binPolygon.Get(i).m_x -= xbinmin;
		binPolygon.Get(i).m_y -= ybinmin;
		double a = binPolygon.Get(i).m_x;
	}


	double binPolygonWidth = xbinmax - xbinmin;
	double binPolygonHeight = ybinmax - ybinmin;

	if (GeometryUtil::PolygonArea(binPolygon) > 0) 
	{
		binPolygon.Reverse();
	}

	for (int i = 0; i < tree.size(); i++) 
	{
		Segments start = tree.at(i).Get(0);
		Segments end = tree.at(i).Get(tree.at(i).Size() - 1);
		if (start == end || GeometryUtil::AlmostEqual(start.m_x, end.m_x) && GeometryUtil::AlmostEqual(start.m_y, end.m_y))
		{
			tree.at(i).Pop();
		}
		if (GeometryUtil::PolygonArea(tree.at(i)) > 0) 
		{
			tree.at(i).Reverse();
		}
	}

	m_launchcount = 0;
	Result best;
	for (int i = 0; i < m_loopCount; i++)
	{

		Result result = LaunchWorkers(tree, binPolygon, m_config);

		std::cout << "fitness: " << result.m_fitness << endl;

		if (i == 0) 
		{
			best = result;
		}
		else 
		{
			if (best.m_fitness > result.m_fitness)
			{
				best = result;
			}
		}
	}
	double sumarea = 0;
	double totalarea = 0;
	for (int i = 0; i < best.m_placements.size(); i++)
	{
		totalarea += abs(GeometryUtil::PolygonArea(binPolygon));
		for (int j = 0; j < best.m_placements.at(i).size(); j++)
		{
			sumarea += abs(GeometryUtil::PolygonArea(tree.at(best.m_placements.at(i).at(j).m_id)));
		}
	}
	double rate = (sumarea / totalarea) * 100;
	vector<vector<Placement>> appliedPlacement = ApplyPlacement(best, tree);
	return appliedPlacement;
}


Result Nest::LaunchWorkers(vector<NestPath> tree, NestPath binPolygon, Config config)
{
	m_launchcount++;
	if (m_GA == NULL)
	{
		vector<NestPath> adam;
		NestPath nestPath;
		for (int i = 0; i < tree.size(); i++)
		{
			nestPath = tree.at(i);
			NestPath *clone = new NestPath(nestPath);
			adam.push_back(*clone);
			adam.at(i).m_area = GeometryUtil::PolygonArea(nestPath);
		}
        std::sort(adam.begin(), adam.end(), LocMinSorter());
		m_GA = new GeneticAlgorithm(adam, binPolygon, config);
	}

	Individual *individual = NULL;
	for (int i = 0; i < m_GA->m_population->size(); i++)
	{
		if (m_GA->m_population->at(i).GetFitness() < 0)
		{
			individual = &(m_GA->m_population->at(i));
			break;
		}
	}
	if (m_launchcount > 1 && individual == NULL)
	{
		m_GA->Generation();
		*individual = m_GA->m_population->at(1);
	}

	vector<NestPath> placelist = individual->GetPlacement();
	vector<int> rotations = individual->GetRotation();

	vector<int> ids;
	for (int i = 0; i < placelist.size(); i++) 
	{
		ids.push_back(placelist.at(i).GetId());
		placelist.at(i).SetRotation(rotations.at(i));
	}
	vector<NfpPair> nfpPairs;
	NfpKey key;

	for (int i = 0; i < placelist.size(); i++) 
	{
		NestPath part = placelist.at(i);
		key = NfpKey(binPolygon.GetId(), part.GetId(), true, 0, part.GetRotation());
		nfpPairs.push_back(NfpPair(binPolygon, part, key));
		for (int j = 0; j < i; j++) 
		{
			NestPath placed = placelist.at(j);
			NfpKey keyed(placed.GetId(), part.GetId(), false, rotations.at(j), rotations.at(i));
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
		string tkey = m_gson->ToJson(Nfp.GetKey());
		m_nfpCache->insert(make_pair(tkey, *Nfp.m_value));
	}


	Placementworker worker(binPolygon, config, *m_nfpCache);
	vector<NestPath> placeListSlice;

	for (int i = 0; i < placelist.size(); i++) 
	{
		placeListSlice.push_back(placelist.at(i));
	}
	vector<vector<NestPath>> data;
	data.push_back(placeListSlice);
	vector<Result> placements;
	for (int i = 0; i < data.size(); i++) 
	{
		Result result = worker.PlacePaths((data.at(i)));
		std::cout << "result.m_fitness: " << result.m_fitness << endl;
		placements.push_back(result);
	}
	if (placements.size() == 0) 
	{
		Result bestResult1;
		return bestResult1;
	}
	individual->m_fitness = placements.at(0).m_fitness;
	Result bestResult = placements.at(0);
	for (int i = 1; i < placements.size(); i++) 
	{
		if (placements.at(i).m_fitness < bestResult.m_fitness)
		{
			bestResult = placements.at(i);
		}
	}
	return bestResult;
}

vector<vector<Placement>> Nest::ApplyPlacement(Result best, vector<NestPath> tree)
{
	vector<vector<Placement>> applyPlacement;
	for (int i = 0; i < best.m_placements.size(); i++)
	{
		vector<Placement> binTranslate;
		for (int j = 0; j < best.m_placements.at(i).size(); j++)
		{
			Vector v = best.m_placements.at(i).at(j);
			NestPath nestPath = tree.at(v.m_id);
			NestPath child;
			//for (NestPath child : nestPath.getChildren()) 
			for (int i = 0; i < nestPath.GetChildren()->size(); i++)
			{
				child = nestPath.GetChildren()->at(i);
				Placement chPlacement(child.m_bid, Segments(v.m_x, v.m_y), v.m_rotation, child.m_name);
				binTranslate.push_back(chPlacement);
			}
			Placement placement(nestPath.m_bid, Segments(v.m_x, v.m_y), v.m_rotation, nestPath.m_name);
			binTranslate.push_back(placement);
		}
		applyPlacement.push_back(binTranslate);
	}
	return applyPlacement;
}


vector<int> Nest::CheckIfCanBePlaced(NestPath binPolygon, vector<NestPath> tree)
{
	vector<int> CanBePlacdPolygonIndex;
	Bound binBound = GeometryUtil::GetPolygonBounds(binPolygon);
	for (int i = 0; i < tree.size(); i++) 
	{
		NestPath nestPath = tree.at(i);
		if (nestPath.GetRotation() == 0) 
		{
			Bound bound = GeometryUtil::GetPolygonBounds(nestPath);
			if (bound.m_width < binBound.m_width && bound.m_height < binBound.m_height)
			{
				CanBePlacdPolygonIndex.push_back(i);
				continue;
			}
		}
		else {
			for (int j = 0; j < nestPath.GetRotation(); j++) 
			{
				Bound rotatedBound = GeometryUtil::RotatePolygon(nestPath, (360 / nestPath.GetRotation()) * j);
				if (rotatedBound.m_width < binBound.m_width && rotatedBound.m_height < binBound.m_height)
				{
					CanBePlacdPolygonIndex.push_back(i);
					break;
				}
			}
		}
	}
	return CanBePlacdPolygonIndex;
}



void Nest::Add(NestPath np) 
{
	m_parts.push_back(np);
}

NestPath Nest::GetBinPath() 
{
	return m_binPath;
}

vector<NestPath> Nest::GetParts()
{
	return m_parts;
}

void Nest::SetBinPath(NestPath binPath_para)
{
	m_binPath = binPath_para;
}

void Nest::SetParts(vector<NestPath> parts)
{
	m_parts = parts;
}

Config Nest::GetConfig() 
{
	return m_config;
}

void Nest::SetConfig(Config config) 
{
	m_config = config;
}


