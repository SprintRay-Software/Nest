#pragma once
#include "NestPath.h"
#include "NestCoor.h"
#include "ClipperCoor.h"
#include "clipper.hpp"

using namespace ClipperLib;

class CommonUtil
{
public:
    static NestPath path2NestPath(Path path);
    static Path nestPath2Path(NestPath nestPath);
    static ClipperCoor toClipperCoor(double x, double y);
    static NestCoor toNestCoor(long x, long y);
    static void changePosition(NestPath binPath, vector<NestPath> polys);
    static int toTree(vector<NestPath> &listNestPath, int idStart);
    static NestPath clipperToNestPath(Path polygon);
    static void ramerDouglasPeucker(Path &in,double epsilon,Path &out);
    static double perpendicularDistance(IntPoint pt,IntPoint lineStart,IntPoint lineEnd);
    /**
     * @brief   modified by wangjx 2021.03.02
     * change "t" to "&t" for repairing a bug
     *
     * @param t
     * @param offset
     */
    static void offsetTree(vector<NestPath> &t, double offset);
    static vector<NestPath> polygonOffset(NestPath polygon, double offset);
    /**
     * @brief 创建树
     *
     * @param parts
     * @param curveTolerance
     * @return vector<NestPath>
     */
    static vector<NestPath> buildTree(vector<NestPath> parts, double curveTolerance);

private:
    static void addPoint(long x, long y, Path path);
};
