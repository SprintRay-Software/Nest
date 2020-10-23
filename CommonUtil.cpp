#include "CommonUtil.h"
#include "Config.h"
#include "GeometryUtil.h"

NestPath CommonUtil::Path2NestPath(Path path)
{
	NestPath nestPath;
	for (int i = 0; i < path.size(); i++) 
	{
		IntPoint lp = path.at(i);
		NestCoor coor = CommonUtil::ToNestCoor(lp.X, lp.Y);
		nestPath.Add(Segments(coor.GetX(), coor.GetY()));
	}
	return nestPath;
}

Path CommonUtil::NestPath2Path(NestPath nestPath) 
{
	Path path;
	//for (Segments s : nestPath.getSegments()) 
	for (int i = 0; i < nestPath.GetSegments()->size(); i++)
	{
		ClipperCoor coor = CommonUtil::ToClipperCoor(nestPath.Get(i).GetX(), nestPath.Get(i).GetY());
		IntPoint lp(coor.GetX(), coor.GetY());
		path.push_back(lp);
	}
	return path;
}

/**
	* 坐标转换
	* @param x
	* @param y
	* @return
	*/
ClipperCoor CommonUtil::ToClipperCoor(double x, double y)
{
	return ClipperCoor((long)(x*Config::m_cliiper_scale), (long)(y * Config::m_cliiper_scale));
}

/**
	* 坐标转换
	* @param x
	* @param y
	* @return
	*/
NestCoor CommonUtil::ToNestCoor(long x, long y)
{
	return NestCoor(((double)x / Config::m_cliiper_scale), ((double)y / Config::m_cliiper_scale));
}


/**
	* 为Clipper下的Path添加点
	* @param x
	* @param y
	* @param path
	*/
void CommonUtil::AddPoint(long x, long y, Path path) 
{
	IntPoint ip(x, y);
	path.push_back(ip);
}


/**
	* binPath是作为底板的NestPath , polys则为板件的Path列表
	* 这个方法是为了将binPath和polys在不改变自身形状，角度的情况下放置在一个坐标系内，保证两两之间不交叉
	* @param binPath
	* @param polys
	*/
void CommonUtil::ChangePosition(NestPath binPath, vector<NestPath> polys)
{
	return;
}

/**
	*  将NestPath列表转换成父子关系的树
	* @param list
	* @param idstart
	* @return
	*/
int CommonUtil::ToTree(vector<NestPath>& list_para, int idstart)
{
	vector<NestPath> parents;
	int id = idstart;
	/**
		* 找出所有的内回环
		*/
	//list<NestPath>::iterator iter = list_para.begin();
	for (int i = 0; i < list_para.size(); i++)
	{
		NestPath p = list_para.at(i);
		//advance(iter, i);
		//NestPath p = *iter;
		bool isChild = false;
		for (int j = 0; j < list_para.size(); j++)
		{
			if (j == i) {
				continue;
			}
			for (int k = 0; k < p.GetSegments()->size(); k++)
			{
				if (GeometryUtil::PointInPolygon(p.GetSegments()->at(0), list_para.at(j)) == true)
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
					list_para.at(j).GetChildren()->push_back(p);
					p.SetParent(&list_para.at(j));
					isChild = true;
					break;
				}
			}
			//if (GeometryUtil::pointInPolygon(p.getSegments()->at(0), list_para.at(j)) == true)
			//{
			//	list_para.at(j).getChildren()->push_back(p);
			//	p.setParent(&list_para.at(j));
			//	isChild = true;
			//	break;
			//}
		}
		if (!isChild) 
		{
			parents.push_back(p);
		}
	}
	/**
		*  将内环从list列表中去除
		*/
	for (int i = 0; i < list_para.size(); i++)
	{
		//if (parents->indexOf(list_para.at(i)) < 0)
		//{
		//	list_para.erase(list_para.begin() + i);
		//	i--;
		//}

		for (auto iter = parents.begin(); iter != parents.end();)
		{
			if (*iter == list_para.at(i))
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
		parents.at(i).SetId(id);
		id++;
	}

	for (int i = 0; i < parents.size(); i++)
	{
		if (parents.at(i).GetChildren()->size() > 0)
		{
			id = ToTree(*(parents.at(i).GetChildren()), id);
		}
	}
	list_para = parents;
	return id;
}

NestPath CommonUtil::ClipperToNestPath(Path polygon) 
{
	NestPath normal;
	for (int i = 0; i < polygon.size(); i++) 
	{
		NestCoor nestCoor = ToNestCoor(polygon.at(i).X, polygon.at(i).Y);
		normal.Add(Segments(nestCoor.GetX(), nestCoor.GetY()));
	}
	return normal;
}

void CommonUtil::OffsetTree(vector<NestPath> t, double offset)
{
	for (int i = 0; i < t.size(); i++) 
	{
		vector<NestPath> offsetPaths = PolygonOffset(t.at(i), offset);
		if (offsetPaths.size() == 1) 
		{
			t.at(i).Clear();
			NestPath from = offsetPaths.at(0);

			//for (Segments s : from.getSegments()) 
			//{
			//	t.at(i).add(s);
			//}
			vector<Segments>* temp_segments = from.GetSegments();
			for (int i = 0; i < temp_segments->size(); i++)
			{
				t.at(i).Add(temp_segments->at(i));
			}
		}
		if (t.at(i).GetChildren()->size() > 0) 
		{

			OffsetTree(*(t.at(i).GetChildren()), -offset);
		}
	}
}

vector<NestPath> CommonUtil::PolygonOffset(NestPath polygon, double offset)
{
	vector<NestPath> result;
	if (offset == 0 || GeometryUtil::AlmostEqual(offset, 0)) 
	{
		/**
			* return EmptyResult
			*/
		return result;
	}
	Path p;
	//for (Segments s : polygon.getSegments()) 
	for (int i = 0; i < polygon.GetSegments()->size(); i++)
	{
		ClipperCoor cc = CommonUtil::ToClipperCoor(polygon.Get(i).GetX(), polygon.Get(i).GetY());
		p.push_back(IntPoint(cc.GetX(), cc.GetY()));
	}

	int miterLimit = 2;
	ClipperOffset co(miterLimit, polygon.m_config->m_curve_tolerance * Config::m_cliiper_scale);
	co.AddPath(p, JoinType::jtRound, EndType::etClosedPolygon);

	Paths newpaths;
	co.Execute(newpaths, offset * Config::m_cliiper_scale);


	/**
		* 这里的length是1的话就是我们想要的
		*/
	for (int i = 0; i < newpaths.size(); i++) 
	{
		result.push_back(CommonUtil::ClipperToNestPath(newpaths.at(i)));
	}

	if (offset > 0) 
	{
		NestPath from = result.at(0);
		if (GeometryUtil::PolygonArea(from) > 0) 
		{
			from.Reverse();
		}
		from.Add(from.Get(0)); from.GetSegments()->erase(from.GetSegments()->begin());
	}


	return result;
}


/**
	* 对应于JS项目中的getParts
	*/
vector<NestPath> CommonUtil::BuildTree(vector<NestPath> parts, double curve_tolerance)
{
	vector<NestPath> polygons;
	//vector<NestPath> *polygons;
	NestPath cleanPoly;
	for (int i = 0; i < parts.size(); i++) 
	{
		cleanPoly = NestPath::CleanNestPath(parts.at(i));
		cleanPoly.m_bid = parts.at(i).m_bid;
		//cleanPoly->setId(parts.at(i).bid);
		if (cleanPoly.Size() > 2 && abs(GeometryUtil::PolygonArea(cleanPoly)) > curve_tolerance * curve_tolerance)
		{
			cleanPoly.SetSource(i);

			polygons.push_back(cleanPoly);
		}
	}
	CommonUtil::ToTree(polygons, 0);
	return polygons;
}
