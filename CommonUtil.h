#pragma once
#include "NestPath.h"
#include "NestCoor.h"
#include "ClipperCoor.h"
#include "clipper.hpp"

using namespace ClipperLib;

class CommonUtil
{
public:
	static NestPath Path2NestPath(Path path);
	static Path NestPath2Path(NestPath nestPath);
	static ClipperCoor ToClipperCoor(double x, double y);
	static NestCoor ToNestCoor(long x, long y);
	static void ChangePosition(NestPath binPath, vector<NestPath> polys);
	static int ToTree(vector<NestPath> &list_para, int idstart);
	static NestPath ClipperToNestPath(Path polygon);
	static void OffsetTree(vector<NestPath> t, double offset);
	static vector<NestPath> PolygonOffset(NestPath polygon, double offset);
	static vector<NestPath> BuildTree(vector<NestPath> parts, double curve_tolerance);

private:
	static void AddPoint(long x, long y, Path path);
};