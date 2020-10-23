#include "NestPath.h"
#include "CommonUtil.h"


void NestPath::Add(double x, double y)
{
	Add(Segments(x, y));
}

bool NestPath::Equals(NestPath obj)
{
	NestPath nestPath = (NestPath)obj;
	if (m_segments->size() != nestPath.Size())
	{
		return false;
	}
	for (int i = 0; i < m_segments->size(); i++)
	{
		if (!m_segments->at(i).Equals(nestPath.Get(i)))
		{
			return false;
		}
	}
	if (m_children->size() != nestPath.GetChildren()->size())
	{
		return false;
	}
	for (int i = 0; i < m_children->size(); i++)
	{
		if (!m_children->at(i).Equals(nestPath.GetChildren()->at(i)))
		{
			return false;
		}
	}
	return true;
}

Config* NestPath::GetConfig()
{
	return m_config;
}

void NestPath::SetConfig(Config *config)
{
	m_config = config;
}


void NestPath::Pop()
{
	//segments->Remove(segments->size() - 1);
	m_segments->pop_back();
}

void NestPath::Reverse()
{
	vector<Segments> rever;
	for (int i = m_segments->size() - 1; i >= 0; i--)
	{
		rever.push_back(m_segments->at(i));
	}
	m_segments->clear();
	//for (Segments s : rever) 
	for(int i=0; i< rever.size(); i++)
	{
		m_segments->push_back(rever.at(i));
	}
}

Segments& NestPath::Get(int i)
{
	return m_segments->at(i);
}

NestPath NestPath::GetParent()
{
	return *m_parent;
}

void NestPath::SetParent(NestPath *parent)
{
	m_parent = parent;
}

void NestPath::AddChildren(NestPath nestPath)
{
	m_children->push_back(nestPath);
	nestPath.SetParent(this);
}

string NestPath::ToString()
{
	string res = "";
	res += "id = " + to_string(m_id) + " , source = " + to_string(m_source) + " , rotation = " + to_string(m_rotation) + "\n";
	int count = 0;
	//for (Segments s : segments) 
	for (int i = 0; i < m_segments->size(); i++)
	{
		res += "Segments " + to_string(count) + "\n";
		count++;
		res += m_segments->at(i).ToString() + "\n";
	}
	count = 0;
	//for (NestPath nestPath : children) 
	for (int j = 0; j < m_children->size(); j++)
	{
		res += "children " + to_string(count) + "\n";
		count++;
		res += m_children->at(j).ToString();
	}
	return res;
}

vector<NestPath>* NestPath::GetChildren()
{
	return m_children;
}

void NestPath::SetChildren(vector<NestPath> *children)
{
	m_children = children;
}

int NestPath::GetRotation()
{
	return m_rotation;
}

void NestPath::SetRotation(int rotation)
{
	m_rotation = rotation;
}

void NestPath::SetSegments(vector<Segments> *segments)
{
	m_segments = segments;
}

int NestPath::GetSource()
{
	return m_source;
}

void NestPath::SetSource(int source)
{
	m_source = source;
}

NestPath::NestPath()
{
	m_offsetX = 0;
	m_offsetY = 0;
	m_children = new vector<NestPath>();
	m_segments = new vector<Segments>();
	m_config = new Config();
	m_area = 0;
}


NestPath::NestPath(Config *config)
{
	m_offsetX = 0;
	m_offsetY = 0;
	m_children = new vector<NestPath>();
	m_segments = new vector<Segments>();
	m_area = 0;
	m_config = config;
}

NestPath& NestPath::operator=(NestPath srcNestPath)
{
	m_segments = new vector<Segments>();
	//for (Segments Segments : srcNestPath.getSegments()) 
	Segments Segments;
	for (int i = 0; i < srcNestPath.GetSegments()->size(); i++)
	{
		Segments = srcNestPath.GetSegments()->at(i);
		m_segments->push_back(Segments);
	}

	m_id = srcNestPath.m_id;
	m_name = srcNestPath.m_name;
	m_rotation = srcNestPath.m_rotation;
	m_source = srcNestPath.m_source;
	m_offsetX = srcNestPath.m_offsetX;
	m_offsetY = srcNestPath.m_offsetY;
	m_bid = srcNestPath.m_bid;
	m_area = srcNestPath.m_area;
	m_children = new vector<NestPath>();

	//for (NestPath nestPath : srcNestPath.getChildren()) 
	NestPath nestPath;
	for (int i = 0; i < srcNestPath.GetChildren()->size(); i++)
	{
		NestPath child(nestPath);
		child.SetParent(this);
		m_children->push_back(child);
	}
	return *this;
}

NestPath NestPath::CleanNestPath(NestPath srcPath)
{
	/**
		* Convert NestPath 2 Clipper
		*/
	Path path = CommonUtil::NestPath2Path(srcPath);
	Paths simple;
	SimplifyPolygon(path, simple, PolyFillType::pftNonZero);
	if (simple.size() == 0) 
	{
		return NULL;
	}
	Path biggest = simple.at(0);
	double biggestArea = abs(Area(biggest));
	for (int i = 0; i < simple.size(); i++) 
	{
		double area = abs(Area(simple.at(i)));
		if (area > biggestArea) 
		{
			biggest = simple.at(i);
			biggestArea = area;
		}
	}
	CleanPolygon(biggest, srcPath.m_config->m_curve_tolerance * Config::m_cliiper_scale);
	if (biggest.size() == 0)
	{
		return NULL;
	}

	/**
		*  Convert Clipper 2 NestPath
		*/
	NestPath cleanPath;
	cleanPath = CommonUtil::Path2NestPath(biggest);
	cleanPath.m_bid = srcPath.m_bid;
	cleanPath.m_name = srcPath.m_name;
	cleanPath.SetRotation(srcPath.m_rotation);
	return cleanPath;
}

void NestPath::Zerolize()
{
	ZeroX(); ZeroY();
}

void NestPath::ZeroX()
{
	double xMin = DBL_MAX;
	//for (Segments s : segments) 
	for (int i = 0; i < m_segments->size(); i++)
	{
		if (xMin > m_segments->at(i).GetX())
		{
			xMin = m_segments->at(i).GetX();
		}
	}
	//for (Segments s : segments) 
	for (int j = 0; j < m_segments->size(); j++)
	{
		m_segments->at(j).SetX(m_segments->at(j).GetX() - xMin);
	}
}

void NestPath::ZeroY()
{
	double yMin = DBL_MAX;
	//for (Segments s : segments) 
	for (int i = 0; i < m_segments->size(); i++)
	{
		if (yMin > m_segments->at(i).GetY())
		{
			yMin = m_segments->at(i).GetY();
		}
	}
	//for (Segments s : segments) 
	for (int j = 0; j < m_segments->size(); j++)
	{
		m_segments->at(j).SetY(m_segments->at(j).GetY() - yMin);
	}
}

void NestPath::Clear()
{
	m_segments->clear();
}

int NestPath::Size()
{
	return m_segments->size();
}

void NestPath::Add(Segments s)
{
	m_segments->push_back(s);
}

vector<Segments>* NestPath::GetSegments()
{
	return m_segments;
}


int NestPath::GetId()
{
	return m_id;
}

void NestPath::SetId(int id)
{
	m_id = id;
}

double NestPath::GetOffsetX()
{
	return m_offsetX;
}

void NestPath::SetOffsetX(double offsetX)
{
	m_offsetX = offsetX;
}

double NestPath::GetOffsetY()
{
	return m_offsetY;
}

void NestPath::SetOffsetY(double offsetY)
{
	m_offsetY = offsetY;
}

int NestPath::CompareTo(NestPath o)
{
	double area0 = m_area;
	double area1 = o.m_area;
	//if (area0 > area1) 
	//{
	//	return 1;
	//}
	//else if (area0 == area1) 
	//{
	//	return 0;
	//}
	//return -1;
	return area0 - area1;
}

double NestPath::GetMaxY()
{
	double MaxY = DBL_MIN;
	//for (Segments s : segments) 
	for (int i = 0; i < m_segments->size(); i++)
	{
		if (MaxY < m_segments->at(i).GetY())
		{
			MaxY = m_segments->at(i).GetY();
		}
	}
	return MaxY;
}

void NestPath::Translate(double x, double y)
{
	//for (Segments s : segments) 
	for (int i = 0; i < m_segments->size(); i++)
	{
		m_segments->at(i).SetX(m_segments->at(i).GetX() + x);
		m_segments->at(i).SetY(m_segments->at(i).GetY() + y);
	}
}

bool NestPath::operator==(const NestPath& np)
{
	if (m_area == np.m_area)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool NestPath::operator < (const NestPath& rhs)
{
	return CompareTo(rhs) < 0;
}
