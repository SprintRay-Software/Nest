#include "NestPath.h"
#include "CommonUtil.h"


void NestPath::Add(double x, double y)
{
    Add(Segments(x, y));
}

bool NestPath::equals(NestPath obj)
{
    NestPath nestPath = (NestPath)obj;
    if (segments->size() != nestPath.Size())
    {
        return false;
    }
    for (int i = 0; i < segments->size(); i++)
    {
        if (!segments->at(i).equals(nestPath.Get(i)))
        {
            return false;
        }
    }
    if (children->size() != nestPath.getChildren()->size())
    {
        return false;
    }
    for (int i = 0; i < children->size(); i++)
    {
        if (!children->at(i).equals(nestPath.getChildren()->at(i)))
        {
            return false;
        }
    }
    return true;
}

Config* NestPath::getConfig()
{
    return config;
}

void NestPath::setConfig(Config *config)
{
    this->config = config;
}


void NestPath::Pop()
{
    //segments->Remove(segments->size() - 1);
    segments->pop_back();
}

void NestPath::reverse()
{
    vector<Segments> rever;
    for (int i = segments->size() - 1; i >= 0; i--)
    {
        rever.push_back(segments->at(i));
    }
    segments->clear();
    //for (Segments s : rever)
    for(int i=0; i< rever.size(); i++)
    {
        segments->push_back(rever.at(i));
    }
}

Segments& NestPath::Get(int i)
{
    return segments->at(i);
}

NestPath NestPath::getParent()
{
    return *parent;
}

void NestPath::setParent(NestPath *parent)
{
    this->parent = parent;
}

void NestPath::addChildren(NestPath nestPath)
{
    children->push_back(nestPath);
    nestPath.setParent(this);
}

string NestPath::toString()
{
    string res = "";
    res += "id = " + to_string(id) + " , source = " + to_string(source) + " , rotation = " + to_string(rotation) + "\n";
    int count = 0;
    for (int i = 0; i < segments->size(); i++)
    {
        res += "Segments " + to_string(count) + "\n";
        count++;
        res += segments->at(i).toString() + "\n";
    }
    count = 0;
    for (int j = 0; j < children->size(); j++)
    {
        res += "children " + to_string(count) + "\n";
        count++;
        res += children->at(j).toString();
    }
    return res;
}

vector<NestPath>* NestPath::getChildren()
{
    return children;
}

void NestPath::setChildren(vector<NestPath> *children)
{
    this->children = children;
}

int NestPath::getRotation()
{
    return rotation;
}

int NestPath::getRotationNum()
{
    return rotationNum;
}

void NestPath::setRotationNum(int rotationNum)
{
    this->rotationNum = rotationNum;
}

void NestPath::setRotation(int rotation)
{
    this->rotation = rotation;
}

void NestPath::setSegments(vector<Segments> *segments)
{
    this->segments = segments;
}

int NestPath::getSource()
{
    return source;
}

void NestPath::setSource(int source)
{
    this->source = source;
}

NestPath::NestPath()
{
    offsetX = 0;
    offsetY = 0;
    children = new vector<NestPath>();
    segments = new vector<Segments>();
    config = new Config();
    area = 0;
}


NestPath::NestPath(Config *config)
{
    offsetX = 0;
    offsetY = 0;
    children = new vector<NestPath>();
    segments = new vector<Segments>();
    area = 0;
    this->config = config;
}

NestPath& NestPath::operator=(NestPath srcNestPath)
{
    segments = new vector<Segments>();
    Segments Segments;
    for (int i = 0; i < srcNestPath.GetSegments()->size(); i++)
    {
        Segments = srcNestPath.GetSegments()->at(i);
        segments->push_back(Segments);
    }

    id = srcNestPath.id;
    name = srcNestPath.name;
    rotation = srcNestPath.rotation;
    rotationNum = srcNestPath.rotationNum;
    source = srcNestPath.source;
    offsetX = srcNestPath.offsetX;
    offsetY = srcNestPath.offsetY;
    bid = srcNestPath.bid;
    area = srcNestPath.area;
    children = new vector<NestPath>();

    NestPath nestPath;
    for (int i = 0; i < srcNestPath.getChildren()->size(); i++)
    {
        NestPath child(nestPath);
        child.setParent(this);
        children->push_back(child);
    }
    return *this;
}

NestPath NestPath::cleanNestPath(NestPath srcPath)
{
    /**
     * Convert NestPath 2 Clipper
     */
    Path path = CommonUtil::nestPath2Path(srcPath);
    Paths simple;
    /*
     * Clipper::SimplifyPolygon
     * 简化多边形  将自交部分从自身的部分中剔除
     * 会切出多个多边形  选取其中最大的一个多边形
     */
    SimplifyPolygon(path, simple, PolyFillType::pftNonZero);
    if (simple.size() == 0)
    {
        return NULL;
    }
    /*
     * simple[0]是SimplifyPolygon输出的第一个图形轮廓
     *
     */
    Path biggest = simple.at(0);
    double biggestArea = abs(Area(biggest));
    /*
     * 遍历检验最大面积  这个Area求出来的到底是模型面积还是模型和Y轴合计的面积
     */
    for (int i = 0; i < simple.size(); i++)
    {
        double area = abs(Area(simple.at(i)));
        if (area > biggestArea)
        {
            biggest = simple.at(i);
            biggestArea = area;
        }
    }

    CleanPolygon(biggest, srcPath.config->curveTolerance * Config::cliiperScale);
    if (biggest.size() == 0)
    {
        return NULL;
    }

    /**
      *  Convert Clipper 2 NestPath
      */
    NestPath cleanPath;
    cleanPath = CommonUtil::path2NestPath(biggest);
    cleanPath.bid = srcPath.bid;
    cleanPath.name = srcPath.name;
    cleanPath.setRotation(srcPath.rotation);
    return cleanPath;
}

void NestPath::zerolize()
{
    zeroX(); zeroY();
}

void NestPath::zeroX()
{
    double xMin = DBL_MAX;
    for (int i = 0; i < segments->size(); i++)
    {
        if (xMin > segments->at(i).getX())
        {
            xMin = segments->at(i).getX();
        }
    }
    for (int j = 0; j < segments->size(); j++)
    {
        segments->at(j).setX(segments->at(j).getX() - xMin);
    }
}

void NestPath::zeroY()
{
    double yMin = DBL_MAX;
    for (int i = 0; i < segments->size(); i++)
    {
        if (yMin > segments->at(i).getY())
        {
            yMin = segments->at(i).getY();
        }
    }
    for (int j = 0; j < segments->size(); j++)
    {
        segments->at(j).setY(segments->at(j).getY() - yMin);
    }
}

void NestPath::Clear()
{
    segments->clear();
}

int NestPath::Size()
{
    return segments->size();
}

void NestPath::Add(Segments s)
{
    segments->push_back(s);
}

vector<Segments>* NestPath::GetSegments()
{
    return segments;
}


int NestPath::getId()
{
    return id;
}

void NestPath::setId(int id)
{
    this->id = id;
}

double NestPath::getOffsetX()
{
    return offsetX;
}

void NestPath::setOffsetX(double offsetX)
{
    this->offsetX = offsetX;
}

double NestPath::getOffsetY()
{
    return offsetY;
}

void NestPath::setOffsetY(double offsetY)
{
    this->offsetY = offsetY;
}

int NestPath::compareTo(NestPath o)
{
    double area0 = area;
    double area1 = o.area;
    return area0 - area1;
}

double NestPath::getMaxY()
{
    double MaxY = DBL_MIN;
    for (int i = 0; i < segments->size(); i++)
    {
        if (MaxY < segments->at(i).getY())
        {
            MaxY = segments->at(i).getY();
        }
    }
    return MaxY;
}

void NestPath::translate(double x, double y)
{
    for (int i = 0; i < segments->size(); i++)
    {
        segments->at(i).setX(segments->at(i).getX() + x);
        segments->at(i).setY(segments->at(i).getY() + y);
    }
}

bool NestPath::operator==(const NestPath& np)
{
    if (area == np.area)
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
    return compareTo(rhs) < 0;
}
