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
    string toString();

public:
    double x;
    double y;
    int id;
    int rotation;
    Paths nfp;
};
