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
                /*
                 * 如果有多个模型之间相交的部分 那么将相交的部分放到最后去
                 */
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
                /*
                 * 这部分原来是启用的
                 * 如果所有i中的线段都没有和j中的重合  那么就把j的child放进i
                 */
				if (k == p.GetSegments()->size()-1)
				{
                    listNestPath.at(j).getChildren()->push_back(p);
                    p.setParent(&listNestPath.at(j));
					isChild = true;
					break;
				}
			}
		}
        /*
         * 如果有重合 就没有child  就把i放到parents中
         */
		if (!isChild) 
		{
			parents.push_back(p);
		}
        /*
         * 至此parents下包括所有不相交的模型
         */
	}

    for (int i = 0; i < listNestPath.size(); i++)
	{
        /*
         * 这里有错误
         * 从parents中删掉所有原来的模型中有的模型（其实一个也不删除）
         */
		for (auto iter = parents.begin(); iter != parents.end();)
		{
            if (*iter == listNestPath.at(i))    //这步永远不会执行  因为operator==永远返回false
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
            std::cerr<<"There are children in parents["<<i<<"]"<<std::endl;
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

void CommonUtil::ramerDouglasPeucker(Path &pointList, double epsilon, Path &out)
{
    out.clear();
    if (pointList.size() < 2)
    {
        return;
    }

    // Find the point with the maximum distance from line between start and end
    double dmax = 0.0;
    size_t index = 0;
    size_t end = pointList.size() - 1;
    for (size_t i = 1; i < end; i++)
    {
        double d = perpendicularDistance(pointList[i], pointList[0], pointList[end]);
        if (d > dmax)
        {
            index = i;
            dmax = d;
        }
    }

    // If max distance is greater than epsilon, recursively simplify
    if (dmax > epsilon)
    {
        // Recursive call
        vector<IntPoint> recResults1;
        vector<IntPoint> recResults2;
        vector<IntPoint> firstLine(pointList.begin(), pointList.begin() + index + 1);
        vector<IntPoint> lastLine(pointList.begin() + index, pointList.end());
        ramerDouglasPeucker(firstLine, epsilon, recResults1);
        ramerDouglasPeucker(lastLine, epsilon, recResults2);

        // Build the result list
        out.assign(recResults1.begin(), recResults1.end() - 1);
        out.insert(out.end(), recResults2.begin(), recResults2.end());
        if (out.size() < 2)
        {
            return;
        }
    }
    else
    {
        //Just return start and end points
        out.clear();
        out.push_back(pointList[0]);
        out.push_back(pointList[end]);
    }
}

double CommonUtil::perpendicularDistance(IntPoint pt, IntPoint lineStart, IntPoint lineEnd)
{
    double dx = lineEnd.X - lineStart.X;
    double dy = lineEnd.Y - lineStart.Y;

    //Normalise
    double mag = pow(pow(dx, 2.0) + pow(dy, 2.0), 0.5);
    if (mag > 0.0)
    {
        dx /= mag; dy /= mag;
    }

    double pvx = pt.X - lineStart.X;
    double pvy = pt.Y - lineStart.Y;

    //Get dot product (project pv onto normalized direction)
    double pvdot = dx * pvx + dy * pvy;

    //Scale line direction vector
    double dsx = pvdot * dx;
    double dsy = pvdot * dy;

    //Subtract this from pv
    double ax = pvx - dsx;
    double ay = pvy - dsy;

    return pow(pow(ax, 2.0) + pow(ay, 2.0), 0.5);
}

void CommonUtil::offsetTree(vector<NestPath> &t, double offset)
{
    std::vector<NestPath > tmpV;
	for (int i = 0; i < t.size(); i++) 
	{
        vector<NestPath> offsetPaths = polygonOffset(t.at(i), offset);
        if (offsetPaths.size() == 1)
		{
            tmpV.push_back(offsetPaths[0]);
		}
        if (t.at(i).getChildren()->size() > 0)
		{
            //add wangjx
            std::cout<<"tree getChildren :"<<t.at(i).getChildren()->size()<<std::endl;
            offsetTree(*(t.at(i).getChildren()), -offset);
		}
	}
    for(int i=0;i<tmpV.size();i++)
    {
        t[i].Clear();
        t[i]=std::move(tmpV[i]);
    }
}

vector<NestPath> CommonUtil::polygonOffset(NestPath polygon, double offset)
{
	vector<NestPath> result;
    if (offset == 0 || GeometryUtil::almostEqual(offset, 0))
	{
		/**
          * return EmptyResult  返回一个空vector  这里是错误的
          * 应该原路返回
          */
        std::cout<<"GeometryUtil::almostEqual(offset, 0)"<<std::endl;
        result.emplace_back(polygon);
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
    co.AddPath(p, JoinType::jtMiter, EndType::etClosedPolygon);
    Paths newPaths;
    co.Execute(newPaths, offset * Config::cliiperScale);
    Path tmppath;
    for (int i = 0; i < newPaths.size(); i++)
	{
        ramerDouglasPeucker(newPaths[i],15,tmppath);
        NestPath returnNest=CommonUtil::clipperToNestPath(tmppath);
        NestPath tmp=polygon;
        tmp.setSegments(returnNest.GetSegments());
        result.push_back(std::move(tmp));
	}
//  //modified by wangjx
    if (offset > 0)
    {
        NestPath from = result.at(0);
        if (GeometryUtil::polygonArea(from) > 0)
        {
            from.reverse();
        }
        from.Add(from.Get(0)); from.GetSegments()->erase(from.GetSegments()->begin());
    }
//    for(auto &p:result)
//    {
//        std::cout<<"polygonOffset"<<polygon.getRotation()<<std::endl;
//        p.setRotation(polygon.getRotation());
//        p.setConfig(polygon.getConfig());
//        p.setId(polygon.getId());
//        p.name=polygon.name;
//        p.coor_x=polygon.coor_x;
//        p.coor_y=polygon.coor_y;
//        p.offsetX=poly
//    }
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
