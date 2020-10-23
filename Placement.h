#pragma once
#include "Segments.h"

class Placement
{
public:
	Placement(int bid, Segments translate, double rotate, string name);
	Placement();

public:
	int m_bid;
	Segments m_translate;
	double m_rotate;
	string m_name;
	double m_coor_x;
	double m_coor_y;
	bool m_istranslate;
};