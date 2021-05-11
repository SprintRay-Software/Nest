#ifndef AUTOPLACE_H
#define AUTOPLACE_H

#include "AutoPlace_global.h"
#include <string>
#include <vector>
#include <QFileInfo>
#include "NestPath.h"
#include "Placement.h"
#include <QVector>
#include "CommonUtil.h"
#include "Nest.h"
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <qDebug>
#include <QTextCodec>
#include <iostream>
#include <fstream>
#include <sstream>
double renderScale = 0.095;
class AUTOPLACE_EXPORT AutoPlace
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
    static bool read(std::string fileName, std::vector<NestPath> &polygons, double margin, int rotateNum);
    /**
     * @brief offsetToZero   offset Path minx to 0 , miny to 0
     * @param _Segments
     * @param minx      origin minx
     * @param miny      origin miny
     */
    static void offsetToZero(std::vector<Segments>* _Segments,double minx,double miny);
    /**
     * @brief zoomBorderToNormalSize
     * Zoom Border Size Same AS 3D Model and the median is same as 3D Model too
     * @param Paths
     */
    static void zoomBorderToNormalSize(std::vector<NestPath> &Paths);
    static std::vector<double> getMinMaxForSegments(std::vector<Segments>&);
    static std::vector<std::vector<Placement>> translateCoor(double margin, double population, double length, double width, double offset, double para, int rotateNum);
    /**
     * @brief translateCoor
     * @param margin        15.7895
     * @param population    10
     * @param length        1920
     * @param width         1080
     * @param offset        0
     * @param para          0.095
     * @param rotateNum
     * @return
     */static void translate(std::vector<NestPath> &binPath, std::vector<std::vector<Placement>> place);
    static QFileInfoList findFiles(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &suffixInfoList);
};

#endif // AUTOPLACE_H
