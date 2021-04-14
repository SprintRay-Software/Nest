#ifndef NESTPATH_H
#define NESTPATH_H
#include <iostream>
#include <string>
#include <list>
#include "Segments.h"
#include "clipper.hpp"
#include "Config.h"

#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308
using namespace std;
using namespace ClipperLib;


class NestPath
{
public:
    NestPath();
    NestPath(Config *config);
    NestPath& operator=(NestPath srcNestPath);
    void Add(double x, double y);
    bool equals(NestPath obj);
    Config* getConfig();
    void setConfig(Config *config);
    void Pop();
    void reverse();
    Segments& Get(int i);
    NestPath getParent();
    void setParent(NestPath *parent);
    void addChildren(NestPath nestPath);
    string toString();
    vector<NestPath>* getChildren();
    void setChildren(vector<NestPath> *children);
    int getRotation();
    int getRotationNum();
    void setRotationNum(int rotationNum);
    void setRotation(int rotation_para);
    void setSegments(vector<Segments> *segments);
    int getSource();
    void setSource(int source);
    static NestPath cleanNestPath(NestPath srcPath);
    void zerolize();
    void Clear();
    int Size();
    void Add(Segments s);
    vector<Segments>* GetSegments();
    int getId();
    void setId(int id_para);
    double getOffsetX();
    void setOffsetX(double offsetX);
    double getOffsetY();
    void setOffsetY(double offsetY);
    int compareTo(NestPath o);
    double getMaxY();
    void translate(double x, double y);
    bool operator==(const NestPath& np);
    bool operator < (const NestPath& rhs);

private:
    void zeroX();
    void zeroY();

public:
    double offsetX;
    double offsetY;
    Config *config;
    double area;
    int bid;
    vector<Segments> *segments;
    string name;
    double coor_x;
    double coor_y;

private:

    vector<NestPath> *children;
    NestPath *parent;
    int id;
    int source;
    int rotation;
    int rotationNum;
};

#endif // NESTPATH_H
