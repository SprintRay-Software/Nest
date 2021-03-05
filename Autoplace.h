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
    /**
     * @brief 这一步不需要读文件来改变数据
     * 目标：重写改为传入mesh结构的数据地址，然后直接转换
     *
     * @param fileName
     * @param polygons
     * @param margin
     * @param rotateNum
     * @return bool
     */
    static bool read(string fileName, vector<NestPath> &polygons, double margin, int rotateNum);
    static vector<vector<Placement>> translateCoor(double margin, double population, double length, double width, double offset, double para, int rotateNum);
    static void translate(vector<NestPath> &binPath, vector<vector<Placement>> place);
    static QFileInfoList findFiles(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &suffixInfoList);
};
