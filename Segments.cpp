#include "Segments.h"
#include "Config.h"


bool Segments::Equals(Segments obj)
{
	Segments s = (Segments)obj;
	if (m_x == s.m_x && m_y == s.m_y)
	{
		return true;
	}
	return false;
}

Segments::Segments()
{
}


//Segments::Segments(Segments srcSeg)
//{
//	x = srcSeg.x;
//	y = srcSeg.y;
//}

Segments::Segments(int x, int y)
{
	m_x = (double)x;
	m_y = (double)y;
}

Segments::Segments(double x1, double y1)
{
	int Ix = (int)(x1 * Config::m_cliiper_scale);
	int Iy = (int)(y1 * Config::m_cliiper_scale);

	m_x = (double)Ix*1.0 / Config::m_cliiper_scale;
	m_y = (double)Iy *1.0 / Config::m_cliiper_scale;
}

string Segments::ToString()
{
	return "x = " + to_string(m_x) + ", y = " + to_string(m_y);
}

bool Segments::isMarked()
{
	return m_marked;
}

void Segments::SetMarked(bool marked)
{
	m_marked = marked;
}

Segments* Segments::GetStart()
{
	return m_start;
}

void Segments::SetStart(Segments *start)
{
	m_start = start;
}

Segments* Segments::GetEnd()
{
	return m_end;
}

void Segments::SetEnd(Segments *end)
{
	m_end = end;
}

double Segments::GetX()
{
	return m_x;
}

void Segments::SetX(double x)
{
	int lx = (int)(m_x * Config::m_cliiper_scale);
	x = lx * 1.0 / Config::m_cliiper_scale;
}

double Segments::GetY()
{
	return m_y;
}

void Segments::SetY(double y)
{
	int ly = (int)(m_y * Config::m_cliiper_scale);
	y = ly * 1.0 / Config::m_cliiper_scale;
}

Segments& Segments::operator=(const Segments srcSeg)
{
	m_x = srcSeg.m_x;
	m_y = srcSeg.m_y;
	return *this;
}

bool Segments::operator!=(const Segments srcSeg)
{
	if (m_x != srcSeg.m_x || m_y != srcSeg.m_y)
		return true;
	else
		return false;
}

bool Segments::operator==(const Segments srcSeg)
{
	if (m_x == srcSeg.m_x && m_y == srcSeg.m_y)
		return true;
	else
		return false;
}