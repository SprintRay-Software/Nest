#pragma once
#include "string"

using namespace std;

class Bound
{
public:
	Bound(double xmin, double ymin, double width, double height);
	Bound();
	double GetXmin();
	void SetXmin(double xmin);
	double GetYmin();
	void SetYmin(double ymin);
	double GetWidth();
	void SetWidth(double width);
	double GetHeight();
	void SetHeight(double height);
	string ToString();

public:
	double m_xmin;
	double m_ymin;
	double m_width;
	double m_height;
};