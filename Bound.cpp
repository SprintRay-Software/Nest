#include "Bound.h"


Bound::Bound(double xmin, double ymin, double width, double height)
{
	m_xmin = xmin;
	m_ymin = ymin;
	m_width = width;
	m_height = height;
}

Bound::Bound()
{
}

double Bound::GetXmin()
{
	return m_xmin;
}

void Bound::SetXmin(double xmin)
{
	m_xmin = xmin;
}

double Bound::GetYmin()
{
	return m_ymin;
}

void Bound::SetYmin(double ymin) 
{
	m_ymin = ymin;
}

double Bound::GetWidth()
{
	return m_width;
}

void Bound::SetWidth(double width)
{
	m_width = width;
}

double Bound::GetHeight()
{
	return m_height;
}

void Bound::SetHeight(double height)
{
	m_height = height;
}

string Bound::ToString()
{
	return "xmin = " + to_string(m_xmin) + " , ymin = " + to_string(m_ymin) + " , width = " + to_string(m_width) + ", height = " + to_string(m_height);
}

