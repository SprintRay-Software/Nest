#include "CommonUtil.h"
#include "Config.h"
#include "GeometryUtil.h"

NestPath CommonUtil::path2NestPath(Path path)
{
	NestPath nestPath;
	for (int i = 0; i < path.size(); i++) 
	{
		IntPoint lp = path.at(i);
        NestCoor coor = CommonUtil::toNestCoor(lp.X, lp.Y);
        nestPath.Add(Segments(coor.getX(), coor.getY()));
	}
	return nestPath;
}

Path CommonUtil::nestPath2Path(NestPath nestPath)
{
	Path path;
	for (int i = 0; i < nestPath.GetSegments()->size(); i++)
	{
        ClipperCoor coor = CommonUtil::toClipperCoor(nestPath.Get(i).getX(), nestPath.Get(i).getY());
        IntPoint lp(coor.getX(), coor.getY());
		path.push_back(lp);
	}
	return path;
}

ClipperCoor CommonUtil::toClipperCoor(double x, double y)
{
    return ClipperCoor((long)(x*Config::cliiperScale), (long)(y * Config::cliiperScale));
}

NestCoor CommonUtil::toNestCoor(long x, long y)
{
    return NestCoor(((double)x / Config::cliiperScale), ((double)y / Config::cliiperScale));
}

void CommonUtil::addPoint(long x, long y, Path path)
{
	IntPoint ip(x, y);
	path.push_back(ip);
}

void CommonUtil::changePosition(NestPath binPath, vector<NestPath> polys)
{
	return;
}

int CommonUtil::toTree(vector<NestPath> &listNestPath, int idStart)
{
	vector<NestPath> parents;
    int id = idStart;
    for (int i = 0; i < listNestPath.size(); i++)
	{
        NestPath p = listNestPath.at(i);
		bool isChild = false;
        for (int j = 0; j < listNestPath.size(); j++)
		{
			if (j == i) {
				continue;
			}
			for (int k = 0; k < p.GetSegments()->size(); k++)
			{
                if (GeometryUtil::pointInPolygon(p.GetSegments()->at(0), listNestPath.at(j)) == true)
				{
					Segments tmep = p.GetSegments()->at(0);
					p.GetSegments()->erase(p.GetSegments()->begin());
					p.GetSegments()->push_back(tmep);
					break;
				}
				else
				{
					break;
				}
				if (k == p.GetSegments()->size()-1)
				{
                    listNestPath.at(j).getChildren()->push_back(p);
                    p.setParent(&listNestPath.at(j));
					isChild = true;
					break;
				}
			}
		}
		if (!isChild) 
		{
			parents.push_back(p);
		}
	}

    for (int i = 0; i < listNestPath.size(); i++)
	{
		for (auto iter = parents.begin(); iter != parents.end();)
		{
            if (*iter == listNestPath.at(i))
			{
				iter = parents.erase(iter);
				i--;
				break;
			}
			else
			{
				iter++;
			}
		}
	}

	for (int i = 0; i < parents.size(); i++)
	{
        parents.at(i).setId(id);
		id++;
	}

	for (int i = 0; i < parents.size(); i++)
	{
        if (parents.at(i).getChildren()->size() > 0)
		{
            id = toTree(*(parents.at(i).getChildren()), id);
		}
	}
    listNestPath = parents;
	return id;
}

NestPath CommonUtil::clipperToNestPath(Path polygon)
{
	NestPath normal;
	for (int i = 0; i < polygon.size(); i++) 
	{
        NestCoor nestCoor = toNestCoor(polygon.at(i).X, polygon.at(i).Y);
        normal.Add(Segments(nestCoor.getX(), nestCoor.getY()));
	}
	return normal;
}

void CommonUtil::offsetTree(vector<NestPath> t, double offset)
{
	for (int i = 0; i < t.size(); i++) 
	{
        vector<NestPath> offsetPaths = polygonOffset(t.at(i), offset);
		if (offsetPaths.size() == 1) 
		{
			t.at(i).Clear();
			NestPath from = offsetPaths.at(0);
            vector<Segments>* tempSegments = from.GetSegments();
            for (int i = 0; i < tempSegments->size(); i++)
			{
                t.at(i).Add(tempSegments->at(i));
			}
		}
        if (t.at(i).getChildren()->size() > 0)
		{

            offsetTree(*(t.at(i).getChildren()), -offset);
		}
	}
}

vector<NestPath> CommonUtil::polygonOffset(NestPath polygon, double offset)
{
	vector<NestPath> result;
    if (offset == 0 || GeometryUtil::almostEqual(offset, 0))
	{
		/**
			* return EmptyResult
			*/
		return result;
	}
	Path p;
	for (int i = 0; i < polygon.GetSegments()->size(); i++)
	{
        ClipperCoor cc = CommonUtil::toClipperCoor(polygon.Get(i).getX(), polygon.Get(i).getY());
        p.push_back(IntPoint(cc.getX(), cc.getY()));
	}

	int miterLimit = 2;
    ClipperOffset co(miterLimit, polygon.config->curveTolerance * Config::cliiperScale);
	co.AddPath(p, JoinType::jtRound, EndType::etClosedPolygon);

    Paths newPaths;
    co.Execute(newPaths, offset * Config::cliiperScale);

    for (int i = 0; i < newPaths.size(); i++)
	{
        result.push_back(CommonUtil::clipperToNestPath(newPaths.at(i)));
	}

	if (offset > 0) 
	{
		NestPath from = result.at(0);
        if (GeometryUtil::polygonArea(from) > 0)
		{
            from.reverse();
		}
		from.Add(from.Get(0)); from.GetSegments()->erase(from.GetSegments()->begin());
	}


	return result;
}

vector<NestPath> CommonUtil::buildTree(vector<NestPath> parts, double curveTolerance)
{
	vector<NestPath> polygons;
	NestPath cleanPoly;
	for (int i = 0; i < parts.size(); i++) 
	{
        cleanPoly = NestPath::cleanNestPath(parts.at(i));
        cleanPoly.bid = parts.at(i).bid;
        if (cleanPoly.Size() > 2 && abs(GeometryUtil::polygonArea(cleanPoly)) > curveTolerance * curveTolerance)
		{
            cleanPoly.setSource(i);

			polygons.push_back(cleanPoly);
		}
	}
    CommonUtil::toTree(polygons, 0);
	return polygons;
}
