#include "ClipperCoor.h"


ClipperCoor::ClipperCoor() 
{
}

ClipperCoor::ClipperCoor(long x, long y) 
{
	m_x = x;
	m_y = y;
}

long ClipperCoor::GetX() 
{
	return m_x;
}

void ClipperCoor::SetX(long x) 
{
	m_x = x;
}

long ClipperCoor::GetY() 
{
	return m_y;
}

void ClipperCoor::SetY(long y) 
{
	m_y = y;
}
