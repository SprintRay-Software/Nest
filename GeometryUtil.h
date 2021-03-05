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
    static bool almostEqual(double a, double b);
    static bool almostEqual(double a, double b, double tolerance);
    static double polygonArea(NestPath polygon);
    static bool onSegment(Segments A, Segments B, Segments p);
    static bool pointInPolygon(Segments point, NestPath polygon);
    static Bound getPolygonBounds(NestPath polygon);
    /*
     * 返回旋转之后的框架
     */
    static Bound rotatePolygon(NestPath polygon, int angle);
    static NestPath rotatePolygon2Polygon(NestPath polygon, int degrees);
    static bool isRectangle(NestPath poly, double tolerance);
    static vector<NestPath> noFitPolygon(NestPath A, NestPath B, bool inside, bool searchEdges);
    static Segments* searchStartPoint(NestPath CA, NestPath CB, bool inside, vector<NestPath> NFP);
    static bool inNfp(Segments p, vector<NestPath>& nfp);
    static double polygonProjectionDistance(NestPath CA, NestPath CB, Segments direction);
    static bool intersect(NestPath CA, NestPath CB);
    static Segments* lineIntersect(Segments A, Segments B, Segments E, Segments F, bool infinite);
    static double polygonSlideDistance(NestPath TA, NestPath TB, Segments direction, bool ignoreNegative);
    static Segments normalizeVector(Segments v);
    static double segmentDistance(Segments A, Segments B, Segments E, Segments F, Segments direction);
    static double pointDistance(Segments p, Segments s1, Segments s2, Segments normal, bool infinite);
    static vector<NestPath>* noFitPolygonRectangle(NestPath A, NestPath B);
    static vector<NestPath> minkowskiDifference(NestPath A, NestPath B);
    static NestPath linearize(Segments p1, Segments p2, double rx, double ry, double angle, int laregearc, int sweep, double tol);
    static DataExchange convertToSvg(Segments center, double rx, double ry, double theta1, double extent, double angleDegrees);
    static DataExchange convertToCenter(Segments p1, Segments p2, double rx, double ry, double angleDgrees, int largearc, int sweep);
    static double degreesToRadians(double angle);
    static double radiansToDegree(double angle);
    static bool withinDistance(Segments p1, Segments p2, double distance);

private:
    static double tol;
};
