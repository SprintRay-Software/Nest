#pragma once
#include "string"
#include "Segments.h"

using namespace std;


class DataExchange
{
public:
	DataExchange(Segments p1, Segments p2, double rx, double ry, double angle, double largearc, int sweep, bool flag);
	DataExchange(Segments center, double rx, double ry, double theta, double extent, double angle, bool flag);
	string ToString();

public:
	Segments m_p1;
	Segments m_p2;
	Segments m_center;
	double m_rx;
	double m_ry;
	double m_theta;
	double m_extent;
	double m_angle;
	double m_largearc;
	int m_sweep;
	bool m_flag;
};