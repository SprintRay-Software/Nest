#pragma once
#include "QFileInfo"
#include "string"
#include "vector"
#include "NestPath.h"
#include "Placement.h"

using namespace std;

class Autoplace
{
public:
    static bool read(string fileName, vector<NestPath> &polygons, double margin, int rotateNum);
    static vector<vector<Placement>> translateCoor(double margin, double population, double length, double width, double offset, double para, int rotateNum);
    static void translate(vector<NestPath> &binPath, vector<vector<Placement>> place);
    static QFileInfoList findFiles(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &suffixInfoList);
};
