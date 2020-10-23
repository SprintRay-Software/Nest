#pragma once
#include "string"
#include "clipper.hpp"

using namespace std;
using namespace ClipperLib;

class Vector
{
public:
	Vector(double x, double y, int id, int rotation);
	Vector(double x, double y, int id, int rotation, Paths nfp);
	Vector();
	string ToString();

public:
	double m_x;
	double m_y;
	int m_id;
	int m_rotation;
	Paths m_nfp;
};