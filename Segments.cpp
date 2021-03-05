#include "Segments.h"
#include "Config.h"


bool Segments::equals(Segments obj)
{
//	Segments s = (Segments)obj;
//    if (x == s.x && y == s.y)
//	{
//		return true;
//	}
//	return false;
    //changed by wangjx
    return  x==obj.getX()&&y==obj.getY()?true:false;
}

Segments::Segments()
{
}

Segments::Segments(int x, int y)
{
    this->x = (double)x;
    this->y = (double)y;
}

Segments::Segments(double x1, double y1)
{
    int Ix = (int)(x1 * Config::cliiperScale);
    int Iy = (int)(y1 * Config::cliiperScale);

    x = (double)Ix*1.0 / Config::cliiperScale;
    y = (double)Iy *1.0 / Config::cliiperScale;
}

string Segments::toString()
{
    return "x = " + to_string(x) + ", y = " + to_string(y);
}

bool Segments::isMarked()
{
    return marked;
}

void Segments::setMarked(bool marked)
{
    this->marked = marked;
}

Segments* Segments::getStart()
{
    return start;
}

void Segments::setStart(Segments *start)
{
    this->start = start;
}

Segments* Segments::getEnd()
{
    return end;
}

void Segments::setEnd(Segments *end)
{
    this->end = end;
}

double Segments::getX()
{
    return x;
}

void Segments::setX(double x)
{
    int lx = (int)(x * Config::cliiperScale);
    x = lx * 1.0 / Config::cliiperScale;
}

double Segments::getY()
{
    return y;
}

void Segments::setY(double y)
{
    int ly = (int)(y * Config::cliiperScale);
    y = ly * 1.0 / Config::cliiperScale;
}

Segments& Segments::operator=(const Segments srcSeg)
{
    x = srcSeg.x;
    y = srcSeg.y;
	return *this;
}

bool Segments::operator!=(const Segments srcSeg)
{
    if (x != srcSeg.x || y != srcSeg.y)
		return true;
	else
		return false;
}

bool Segments::operator==(const Segments srcSeg)
{
    if (x == srcSeg.x && y == srcSeg.y)
		return true;
	else
		return false;
}
