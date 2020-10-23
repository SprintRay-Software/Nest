#include "DataExchange.h"


DataExchange::DataExchange(Segments p1, Segments p2, double rx, double ry, double angle, double largearc, int sweep, bool flag)
{
	m_p1 = p1;
	m_p2 = p2;
	m_rx = rx;
	m_ry = ry;
	m_angle = angle;
	m_largearc = largearc;
	m_sweep = sweep;
	m_flag = flag;
}

DataExchange::DataExchange(Segments center, double rx, double ry, double theta, double extent, double angle, bool flag)
{
	m_center = center;
	m_rx = rx;
	m_ry = ry;
	m_theta = theta;
	m_extent = extent;
	m_angle = angle;
	m_flag = flag;
}

string DataExchange::ToString() 
{
	string s = "";
	if (m_flag) {
		s += " p1 = " + m_p1.ToString() + " p2 = " + m_p2.ToString() + "\n rx = " + to_string(m_rx) + " ry = " + to_string(m_ry) + " angle = " + to_string(m_angle) + " largearc = " + to_string(m_largearc) + " sweep = " + to_string(m_sweep);
	}
	else {
		s += " center = " + m_center.ToString() + "\n rx = " + to_string(m_rx) + " ry = " + to_string(m_ry) + " theta = " + to_string(m_theta) + " extent = " + to_string(m_extent) + " angle = " + to_string(m_angle);
	}
	return s;
}