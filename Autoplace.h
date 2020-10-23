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
    static bool Read(string file_name, vector<NestPath> &temp_polygons, double scale);
	static vector<vector<Placement>> Translate_Coor(double scale, double population);
	static void Translate(vector<NestPath> &binPath_para, vector<vector<Placement>> place);
    static QFileInfoList Find_Files(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &fileList);
	//static string Print_Path();
};
