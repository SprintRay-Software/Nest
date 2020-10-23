#include "Placementworker.h"
#include "GeometryUtil.h"
#include "ClipperCoor.h"
#include "CommonUtil.h"



Gson* Placementworker::m_gson = new Gson();

/**
	*
	* @param binPolygon    底板参数
	* @param config    设置
	* @param nfpCache  nfp列表
	*/
Placementworker::Placementworker(NestPath binPolygon, Config config, map<string, vector<NestPath>> nfpCache)
{
	m_binPolygon = binPolygon;
	m_config = config;
	m_nfpCache = nfpCache;
}

/**
	* 根据板件列表与旋转角列表，通过nfp,计算板件在底板上的位置，并返回这个种群的fitness
	* @param paths
	* @return
	*/
Result Placementworker::PlacePaths(vector<NestPath> &paths)
{
	vector<NestPath> rotated;
	for (int i = 0; i < paths.size(); i++) 
	{
		NestPath r = GeometryUtil::RotatePolygon2Polygon(paths.at(i), paths.at(i).GetRotation());
		r.SetRotation(paths.at(i).GetRotation());
		r.SetSource(paths.at(i).GetSource());
		r.SetId(paths.at(i).GetId());
		rotated.push_back(r);
	}
	paths = rotated;

	vector<vector<Vector>> allplacements;
	double fitness = 0;
	double binarea = abs(GeometryUtil::PolygonArea(m_binPolygon));
	string key = "";
	vector<NestPath> nfp;

	//int L = paths.size();
	while (paths.size() > 0)
	{
		//L--;
		vector<NestPath> *placed = new vector<NestPath>();
		vector<Vector> *placements = new vector<Vector>();

		fitness += 1;
		double minwidth = DBL_MAX;
		for (int i = 0; i < paths.size(); i++) 
		{

			NestPath path = paths.at(i);

			//inner NFP
			key = m_gson->ToJson(NfpKey(-1, path.GetId(), true, 0, path.GetRotation()));

			//if (!nfpCache.containsKey(key)) 
			if (m_nfpCache.find(key) == m_nfpCache.end())
			{
				continue;
			}

			vector<NestPath> binNfp = m_nfpCache[key];



			// ensure exists
			bool error = false;
			for (int j = 0; j < placed->size(); j++) 
			{
				key = m_gson->ToJson(NfpKey(placed->at(j).GetId(), path.GetId(), false, placed->at(j).GetRotation(), path.GetRotation()));
				//if (nfpCache.containsKey(key))
				if (m_nfpCache.find(key) != m_nfpCache.end())
				{
					nfp = (m_nfpCache.find(key)->second);
				}
				else 
				{
					error = true;
					break;
				}
			}
			if (error) 
			{
				continue;
			}

			Vector *position = NULL;
			if (placed->size() == 0)
			{
				// first placement , put it on the lefth
				for (int j = 0; j < binNfp.size(); j++) 
				{
					for (int k = 0; k < binNfp.at(j).Size(); k++) 
					{
						if (position == NULL || binNfp.at(j).Get(k).m_x - path.Get(0).m_x < position->m_x)
						{
							position = new Vector(
								binNfp.at(j).Get(k).m_x - path.Get(0).m_x,
								binNfp.at(j).Get(k).m_y - path.Get(0).m_y,
								path.GetId(),
								path.GetRotation()
							);
						}
					}
				}
				placements->push_back(*position);
				placed->push_back(path);
				continue;
			}

			Paths clipperBinNfp;

			for (int j = 0; j < binNfp.size(); j++) 
			{
				NestPath binNfpj = binNfp.at(j);
				clipperBinNfp.push_back(ScaleUp2ClipperCoordinates(binNfpj));
			}
			//DefaultClipper clipper = new DefaultClipper();
			Clipper clipper;
			Paths combinedNfp;


			for (int j = 0; j < placed->size(); j++) 
			{
				key = m_gson->ToJson(NfpKey(placed->at(j).GetId(), path.GetId(), false, placed->at(j).GetRotation(), path.GetRotation()));
				nfp = m_nfpCache[key];
				if (nfp.empty())
				{
					continue;
				}

				for (int k = 0; k < nfp.size(); k++) 
				{
					Path clone = ScaleUp2ClipperCoordinates(nfp.at(k));
					for (int m = 0; m < clone.size(); m++) 
					{
						long clx = clone.at(m).X;
						long cly = clone.at(m).Y;
						clone.at(m).X = (clx + (long)(placements->at(j).m_x * Config::m_cliiper_scale));
						clone.at(m).Y = (cly + (long)(placements->at(j).m_y * Config::m_cliiper_scale));
					}
					CleanPolygon(clone, 0.0001 * Config::m_cliiper_scale);
					double area = abs(Area(clone));
					if (clone.size() > 2 && area > 0.1 * Config::m_cliiper_scale * Config::m_cliiper_scale)
					{
						clipper.AddPath(clone, PolyType::ptSubject, true);
					}
				}
			}
			if (!clipper.Execute(ClipType::ctUnion, combinedNfp, PolyFillType::pftNonZero))
			{
				continue;
			}

			//difference with bin polygon
			Paths finalNfp;
			clipper.Clear();

			clipper.AddPaths(combinedNfp, PolyType::ptClip, true);
			clipper.AddPaths(clipperBinNfp, PolyType::ptSubject, true);
			if (!clipper.Execute(ClipType::ctDifference, finalNfp, PolyFillType::pftNonZero, PolyFillType::pftNonZero))
			{
				continue;
			}

			//finalNfp = finalNfp->cleanPolygons(0.0001 * Config::CLIIPER_SCALE);
			CleanPolygons(finalNfp, 0.0001 * Config::m_cliiper_scale);
			for (int j = 0; j < finalNfp.size(); j++) 
			{
				double area = abs(Area(finalNfp.at(j)));
				if (finalNfp.at(j).size() < 3 || area < 0.1 * Config::m_cliiper_scale * Config::m_cliiper_scale)
				{
					finalNfp.erase(finalNfp.begin() + j);
					j--;
				}
			}

			if (finalNfp.size() == 0) 
			{
				continue;
			}

			vector<NestPath> f;
			for (int j = 0; j < finalNfp.size(); j++) 
			{
				f.push_back(ToNestCoordinates(finalNfp.at(j)));
			}

			vector<NestPath> finalNfpf = f;
			double minarea = DBL_MIN;
			double minX = DBL_MAX;
			NestPath nf;
			double area = DBL_MIN;
			Vector *shifvector = NULL;
			for (int j = 0; j < finalNfpf.size(); j++) 
			{
				nf = finalNfpf.at(j);
				if (abs(GeometryUtil::PolygonArea(nf)) < 2)
				{
					continue;
				}
				for (int k = 0; k < nf.Size(); k++) 
				{
					NestPath *allpoints = new NestPath();
					for (int m = 0; m < placed->size(); m++) 
					{
						for (int n = 0; n < placed->at(m).Size(); n++) 
						{
							allpoints->Add(Segments(placed->at(m).Get(n).m_x + placements->at(m).m_x,
								placed->at(m).Get(n).m_y + placements->at(m).m_y));
						}
					}
					shifvector = new Vector(
						nf.Get(k).m_x - path.Get(0).m_x,
						nf.Get(k).m_y - path.Get(0).m_y,
						path.GetId(),
						path.GetRotation(),
						combinedNfp
					);
					for (int m = 0; m < path.Size(); m++) 
					{
						allpoints->Add(Segments(path.Get(m).m_x + shifvector->m_x, path.Get(m).m_y + shifvector->m_y));
					}
					Bound rectBounds = GeometryUtil::GetPolygonBounds(*allpoints);

					area = rectBounds.GetWidth() * 2 + rectBounds.GetHeight();
					if (minarea == DBL_MIN
						|| area < minarea
						|| (GeometryUtil::AlmostEqual(minarea, area)
							&& (minX == DBL_MIN || shifvector->m_x < minX)))
					{
						minarea = area;
						minwidth = rectBounds.GetWidth();
						position = shifvector;
						minX = shifvector->m_x;
					}
				}
			}
			if (position != NULL) 
			{

				placed->push_back(path);
				placements->push_back(*position);
			}
		}
		if (minwidth != DBL_MAX) 
		{
			fitness += minwidth / binarea;
		}



		for (int i = 0; i < placed->size(); i++)
		{
			//int index = paths->indexOf(placed->at(i));
			//if (index >= 0) 
			//{
			//	paths->erase(paths->begin() + index);
			//}

			for (auto iter = paths.begin(); iter != paths.end();)
			{
				if ((*iter).m_name == placed->at(i).m_name)
				{
					iter = paths.erase(iter);
					break;
				}
				else
				{
					iter++;
				}
			}
		}

		if (placements != NULL && placements->size() > 0) 
		{
			allplacements.push_back(*placements);
		}
		else 
		{
			//break; // something went wrong
		}

	}
	// there were paths that couldn't be placed
	fitness += 2 * paths.size();
	return Result(allplacements, fitness, paths, binarea);
}


/**
	*  坐标转换，与clipper库交互必须坐标转换
	* @param polygon
	* @return
	*/
Path Placementworker::ScaleUp2ClipperCoordinates(NestPath polygon) 
{
	Path p;
	//for (Segments s : polygon.getSegments()) 
	Segments s;
	for (int i = 0; i < polygon.GetSegments()->size(); i++)
	{
		s = polygon.GetSegments()->at(i);
		ClipperCoor cc = CommonUtil::ToClipperCoor(s.m_x, s.m_y);
		p.push_back(IntPoint(cc.GetX(), cc.GetY()));
	}
	return p;
}

NestPath Placementworker::ToNestCoordinates(Path polygon) 
{
	NestPath clone;
	for (int i = 0; i < polygon.size(); i++) 
	{
		Segments s((double)polygon.at(i).X / Config::m_cliiper_scale, (double)polygon.at(i).Y / Config::m_cliiper_scale);
		clone.Add(s);
	}
	return clone;
}

