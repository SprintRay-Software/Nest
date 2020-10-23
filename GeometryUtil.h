#pragma once
#include <list>
#include <Deque>
#include "Bound.h"
#include "NestPath.h"
#include "SegmentRelation.h"
#include "Placementworker.h"
#include "DataExchange.h"

using namespace std;
using namespace ClipperLib;

class GeometryUtil
{
public:
	static bool AlmostEqual(double a, double b);
	static bool AlmostEqual(double a, double b, double tolerance);
	static double PolygonArea(NestPath polygon);
	static bool OnSegment(Segments A, Segments B, Segments p);
	static bool PointInPolygon(Segments point, NestPath polygon);
	static Bound GetPolygonBounds(NestPath polygon);
	static Bound RotatePolygon(NestPath polygon, int angle);
	static NestPath RotatePolygon2Polygon(NestPath polygon, int degrees);
	static bool isRectangle(NestPath poly, double tolerance);
	static vector<NestPath> NoFitPolygon(NestPath A, NestPath B, bool inside, bool searchEdges);
	static Segments* SearchStartPoint(NestPath CA, NestPath CB, bool inside, vector<NestPath> NFP);
	static bool InNfp(Segments p, vector<NestPath>& nfp);
	static double PolygonProjectionDistance(NestPath CA, NestPath CB, Segments direction);
	static bool Intersect(NestPath CA, NestPath CB);
	static Segments* LineIntersect(Segments A, Segments B, Segments E, Segments F, bool infinite);
	static double PolygonSlideDistance(NestPath TA, NestPath TB, Segments direction, bool ignoreNegative);
	static Segments NormalizeVector(Segments v);
	static double SegmentDistance(Segments A, Segments B, Segments E, Segments F, Segments direction);
	static double PointDistance(Segments p, Segments s1, Segments s2, Segments normal, bool infinite);
	static vector<NestPath>* NoFitPolygonRectangle(NestPath A, NestPath B);
	static vector<NestPath> MinkowskiDifference(NestPath A, NestPath B);
	static NestPath Linearize(Segments p1, Segments p2, double rx, double ry, double angle, int laregearc, int sweep, double tol);
	static DataExchange ConvertToSvg(Segments center, double rx, double ry, double theta1, double extent, double angleDegrees);
	static DataExchange ConvertToCenter(Segments p1, Segments p2, double rx, double ry, double angleDgrees, int largearc, int sweep);
	static double DegreesToRadians(double angle);
	static double RadiansToDegree(double angle);
	static bool WithinDistance(Segments p1, Segments p2, double distance);

private:
	static double m_tol;
};

//double GeometryUtil::TOL = pow(10, -2);