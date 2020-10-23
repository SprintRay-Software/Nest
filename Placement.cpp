#include "Placement.h"


Placement::Placement(int bid, Segments translate, double rotate, string name)
{
	m_bid = bid;
	m_translate = translate;
	m_rotate = rotate;
	m_name = name;
}

Placement::Placement()
{
}

