#include "NestCoor.h"


NestCoor::NestCoor()
{
}

NestCoor::NestCoor(double x, double y)
{
	m_x = x;
	m_y = y;
}

double NestCoor::GetX()
{
	return m_x;
}

void NestCoor::SetX(double x)
{
	m_x = x;
}

double NestCoor::GetY()
{
	return m_y;
}

void NestCoor::SetY(double y)
{
	m_y = y;
}
