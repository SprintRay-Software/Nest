#include "Vector.h"


Vector::Vector(double x, double y, int id, int rotation)
{
	m_x = x;
	m_y = y;
	m_id = id;
	m_rotation = rotation;
}

Vector::Vector(double x, double y, int id, int rotation, Paths nfp)
{
	m_x = x;
	m_y = y;
	m_id = id;
	m_rotation = rotation;
	m_nfp = nfp;
}

Vector::Vector()
{

}


string Vector::ToString()
{
	return  "x = " + to_string(m_x) + " , y = " + to_string(m_y);
}

