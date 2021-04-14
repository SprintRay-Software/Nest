#include "Placementworker.h"
#include "GeometryUtil.h"
#include "ClipperCoor.h"
#include "CommonUtil.h"
#include "qDebug"
#include "map"



Gson* Placementworker::gson = new Gson();

Placementworker::Placementworker(NestPath binPolygon, Config config, map<string, vector<NestPath>> nfpCache)
{
    this->binPolygon = binPolygon;
    this->config = config;
    this->nfpCache = nfpCache;
}

Result Placementworker::placePaths(vector<NestPath> &paths)
{
	vector<NestPath> rotated;
	for (int i = 0; i < paths.size(); i++) 
	{
        NestPath r = GeometryUtil::rotatePolygon2Polygon(paths.at(i), paths.at(i).getRotation());
        r.setRotation(paths.at(i).getRotation());
        r.setSource(paths.at(i).getSource());
        r.setId(paths.at(i).getId());
		rotated.push_back(r);
	}
	paths = rotated;

    vector<vector<Vector>> allPlacements;
	double fitness = 0;
    double binArea = abs(GeometryUtil::polygonArea(binPolygon));
	string key = "";
	vector<NestPath> nfp;

	while (paths.size() > 0)
	{
		vector<NestPath> *placed = new vector<NestPath>();
		vector<Vector> *placements = new vector<Vector>();

		fitness += 1;
        double minWidth = DBL_MAX;
		for (int i = 0; i < paths.size(); i++) 
		{
            //qDebug() << "placePaths: "<< paths.at(i).getId() << ","<< paths.at(i).getRotation()<< endl;
			NestPath path = paths.at(i);

			//inner NFP
            key = gson->toJson(NfpKey(-1, path.getId(), true, 0, path.getRotation()));

            if (nfpCache.find(key) == nfpCache.end())
			{
				continue;
			}

            vector<NestPath> binNfp = nfpCache[key];

			// ensure exists
			bool error = false;
			for (int j = 0; j < placed->size(); j++) 
			{
                key = gson->toJson(NfpKey(placed->at(j).getId(), path.getId(), false, placed->at(j).getRotation(), path.getRotation()));
                if (nfpCache.find(key) != nfpCache.end())
				{
                    nfp = (nfpCache.find(key)->second);
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
                //qDebug() << "placed->size() == 0"<< endl;
				// first placement , put it on the lefth
                for (int j = 0; j < binNfp.size(); j++)
				{
                    for (int k = 0; k < binNfp.at(j).Size(); k++)
					{
                        if (position == NULL || binNfp.at(j).Get(k).x - path.Get(0).x < position->x)
						{
							position = new Vector(
                                binNfp.at(j).Get(k).x - path.Get(0).x,
                                binNfp.at(j).Get(k).y - path.Get(0).y,
                                path.getId(),
                                path.getRotation()
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
                clipperBinNfp.push_back(scaleUp2ClipperCoordinates(binNfpj));
			}
			Clipper clipper;
			Paths combinedNfp;
			for (int j = 0; j < placed->size(); j++) 
			{
                key = gson->toJson(NfpKey(placed->at(j).getId(), path.getId(), false, placed->at(j).getRotation(), path.getRotation()));
                nfp = nfpCache[key];
				if (nfp.empty())
				{
					continue;
				}

				for (int k = 0; k < nfp.size(); k++) 
				{
                    Path clone = scaleUp2ClipperCoordinates(nfp.at(k));
					for (int m = 0; m < clone.size(); m++) 
					{
						long clx = clone.at(m).X;
						long cly = clone.at(m).Y;
                        clone.at(m).X = (clx + (long)(placements->at(j).x * Config::cliiperScale));
                        clone.at(m).Y = (cly + (long)(placements->at(j).y * Config::cliiperScale));
					}
                    CleanPolygon(clone, 0.0001 * Config::cliiperScale);
					double area = abs(Area(clone));
                    if (clone.size() > 2 && area > 0.1 * Config::cliiperScale * Config::cliiperScale)
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
            CleanPolygons(finalNfp, 0.0001 * Config::cliiperScale);
			for (int j = 0; j < finalNfp.size(); j++) 
			{
				double area = abs(Area(finalNfp.at(j)));
                if (finalNfp.at(j).size() < 3 || area < 0.1 * Config::cliiperScale * Config::cliiperScale)
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
                f.push_back(toNestCoordinates(finalNfp.at(j)));
			}

			vector<NestPath> finalNfpf = f;
            double minArea = DBL_MIN;
			double minX = DBL_MAX;
			NestPath nf;
			double area = DBL_MIN;
            Vector *shifVector = NULL;
			for (int j = 0; j < finalNfpf.size(); j++) 
			{
				nf = finalNfpf.at(j);
                if (abs(GeometryUtil::polygonArea(nf)) < 2)
				{
					continue;
				}
				for (int k = 0; k < nf.Size(); k++) 
				{
                    NestPath *allPoints = new NestPath();
					for (int m = 0; m < placed->size(); m++) 
					{
						for (int n = 0; n < placed->at(m).Size(); n++) 
						{
                            allPoints->Add(Segments(placed->at(m).Get(n).x + placements->at(m).x,
                                placed->at(m).Get(n).y + placements->at(m).y));
						}
					}
                    shifVector = new Vector(
                        nf.Get(k).x - path.Get(0).x,
                        nf.Get(k).y - path.Get(0).y,
                        path.getId(),
                        path.getRotation(),
						combinedNfp
					);
					for (int m = 0; m < path.Size(); m++) 
					{
                        allPoints->Add(Segments(path.Get(m).x + shifVector->x, path.Get(m).y + shifVector->y));
					}
                    Bound rectBounds = GeometryUtil::getPolygonBounds(*allPoints);

                    area = rectBounds.getWidth() * 2 + rectBounds.getHeight();
                    if (minArea == DBL_MIN
                        || area < minArea
                        || (GeometryUtil::almostEqual(minArea, area)
                            && (minX == DBL_MIN || shifVector->x < minX)))
					{
                        minArea = area;
                        minWidth = rectBounds.getWidth();
                        position = shifVector;
                        minX = shifVector->x;
					}
				}
			}
			if (position != NULL) 
			{

				placed->push_back(path);
				placements->push_back(*position);
			}
		}
        if (minWidth != DBL_MAX)
		{
            fitness += minWidth / binArea;
		}



		for (int i = 0; i < placed->size(); i++)
		{
			for (auto iter = paths.begin(); iter != paths.end();)
			{
                //qDebug() << "(*iter).name: "<< QString::fromStdString((iter->name))<<"placed->at(i).name: "<<QString::fromStdString(placed->at(i).name) << endl;
                if ((*iter).name == placed->at(i).name)
				{
                    //qDebug() << "paths.erase"<< endl;
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
            allPlacements.push_back(*placements);
		}
		else 
		{
            qDebug() << "something went wrong"<< endl;
            break; // something went wrong
		}

	}
	// there were paths that couldn't be placed
	fitness += 2 * paths.size();
    return Result(allPlacements, fitness, paths, binArea);
}

Path Placementworker::scaleUp2ClipperCoordinates(NestPath polygon)
{
	Path p;
	Segments s;
	for (int i = 0; i < polygon.GetSegments()->size(); i++)
	{
		s = polygon.GetSegments()->at(i);
        ClipperCoor cc = CommonUtil::toClipperCoor(s.x, s.y);
        p.push_back(IntPoint(cc.getX(), cc.getY()));
	}
	return p;
}

NestPath Placementworker::toNestCoordinates(Path polygon)
{
	NestPath clone;
	for (int i = 0; i < polygon.size(); i++) 
	{
        Segments s((double)polygon.at(i).X / Config::cliiperScale, (double)polygon.at(i).Y / Config::cliiperScale);
		clone.Add(s);
	}
	return clone;
}

