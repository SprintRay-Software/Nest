#include <iostream>
#include <fstream>
#include <sstream>
#include "autoplace.h"
#include "Nest.h"
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <qDebug>
#include <QTextCodec>

using namespace std;

#define pi 3.14159265
float coor_Minx_From_Border,coor_Miny_From_Border;
bool AutoPlace::read(string fileName, vector<NestPath> &polygons, double margin, int rotateNum)
{
    ifstream file;
    file.open(string(QString::fromStdString(fileName).toLocal8Bit()), ios::in);
    if (!file.is_open())
    {
       return 0;
    }
    string str;
    while (getline(file, str))
    {
        int count;
        stringstream iss(str);
        iss >> count;
        NestPath tempPolygon;
        string subStr;
        while (count--) {
            subStr.clear();
            iss >> subStr;
            float x, y;
            sscanf(subStr.c_str(), "[%f,%f]", &x, &y);
            //std::cout<<"Test xy "<<x<<"\t"<<y<<std::endl;
            tempPolygon.Add(x, y);
        }
        iss>>subStr;
        float coor_x,coor_y;
        sscanf(subStr.c_str(), "[%f,%f]", &coor_x, &coor_y);
        iss>>subStr;
        sscanf(subStr.c_str(), "[%f,%f]", &coor_Minx_From_Border, &coor_Miny_From_Border);
        //std::cout<<"Test xy "<<coor_x<<"\t"<<coor_y<<std::endl;
        string::size_type iPos = fileName.find_last_of('/') + 1;
        string tempFileName = fileName.substr(iPos, fileName.length() - iPos);
        string name = tempFileName.substr(0, tempFileName.rfind("."));
        tempPolygon.name = name;
        tempPolygon.setRotation(rotateNum);
        tempPolygon.setRotationNum(rotateNum);

        double minX = tempPolygon.segments->at(0).x, maxX = tempPolygon.segments->at(0).x, minY = tempPolygon.segments->at(0).y, maxY = tempPolygon.segments->at(0).y;
        for (int i = 0; i < tempPolygon.segments->size(); i++)
        {
            if (tempPolygon.segments->at(i).x <= minX)
            {
                minX = tempPolygon.segments->at(i).x;
            }
            if (tempPolygon.segments->at(i).x >= maxX)
            {
                maxX = tempPolygon.segments->at(i).x;
            }
            if (tempPolygon.segments->at(i).y <= minY)
            {
                minY = tempPolygon.segments->at(i).y;
            }
            if (tempPolygon.segments->at(i).y >= maxY)
            {
                maxY = tempPolygon.segments->at(i).y;
            }
        }
        tempPolygon.coor_x=coor_x;
        tempPolygon.coor_y=coor_y;
//        ClipperLib::Paths dst_paths;
//        ClipperLib::Path dst_path;
//        for (int i = 0; i < tempPolygon.segments->size(); i++)
//        {
//            dst_path.push_back(ClipperLib::IntPoint(static_cast<ClipperLib::cInt>(tempPolygon.segments->at(i).x * 1000), static_cast<ClipperLib::cInt>(tempPolygon.segments->at(i).y * 1000)));
//        }
//        dst_paths.push_back(dst_path);
//        tempPolygon.segments->clear();
//        for (const auto& dst_path : dst_paths)
//        {
//            for (const auto& point : dst_path)
//            {
//                tempPolygon.segments->push_back(Segments((double)(point.X / 1000.0), (double)(point.Y / 1000.0)));
//            }
//        }
        polygons.push_back(tempPolygon);
    }
}

void AutoPlace::offsetToZero(std::vector<Segments> *_Segments, double minx, double miny)
{
    for(auto &p:*_Segments)
    {
        p.setX(p.getX()-minx);
        p.setY(p.getY()-miny);
    }
}

void AutoPlace::zoomBorderToNormalSize(std::vector<NestPath> &Paths)
{
    for(auto &p:Paths)
    {
        std::vector<double> minMax=getMinMaxForSegments(*p.GetSegments());
        offsetToZero(p.GetSegments(),minMax[0],minMax[1]);
        std::vector<double> minMax2=getMinMaxForSegments(*p.GetSegments());
        double zoomRateX = (p.coor_x*2+coor_Minx_From_Border/1000)/renderScale/minMax2[2];
        double zoomRateY = (p.coor_y*2+coor_Miny_From_Border/1000)/renderScale/minMax2[3];
        for(auto &q:*p.GetSegments())
        {
            q.setX(q.getX()*zoomRateX);
            q.setY(q.getY()*zoomRateY);
        }
        //writeSegmentsTotxt(p,"zoom");
    }
}

std::vector<double> AutoPlace::getMinMaxForSegments(std::vector<Segments> &_Segments)
{
    double minx = std::numeric_limits<double>::max();
    double miny = std::numeric_limits<double>::max();
    double maxx = std::numeric_limits<double>::min();
    double maxy = std::numeric_limits<double>::min();
    for(auto &q:_Segments)
    {
        minx = minx>q.getX()?q.getX():minx;
        miny = miny>q.getY()?q.getY():miny;
        maxx = maxx>q.getX()?maxx:q.getX();
        maxy = maxy>q.getY()?maxy:q.getY();
    }
    std::vector<double> minMax;
    minMax.emplace_back(minx);
    minMax.emplace_back(miny);
    minMax.emplace_back(maxx);
    minMax.emplace_back(maxy);
    return minMax;
};

vector<vector<Placement>> AutoPlace::translateCoor(double margin, double population, double length, double width, double offset, double para, int rotateNum)
{
    vector<NestPath> polygons;
    QString path = QCoreApplication::applicationDirPath() + "/Borderfinder/";
    QFileInfoList fileList;
    findFiles(path, ".txt",fileList);
    QTextCodec *code = QTextCodec::codecForName("GBK");
    string str;
    QString strDir;
    for (int i = 0; i < fileList.size(); i++)
    {
        str = code->fromUnicode(path+fileList.at(i).fileName()).data();
        if(str.find("ConvexHull")!=std::string::npos)
            continue;
        strDir = QString(QString::fromLocal8Bit(str.c_str()));
        read(strDir.toStdString(), polygons, margin, rotateNum);
    }
    zoomBorderToNormalSize(polygons);
    NestPath bin;
    double binWidth = length-2*offset/para;
    double binHeight = width-2*offset/para;
    bin.Add(0, 0);
    bin.Add(binWidth, 0);
    bin.Add(binWidth, binHeight);
    bin.Add(0, binHeight);
    bin.bid = -1;
    Config cfg;
    cfg.spacing = offset;
    cfg.populationSize = population;
    vector<vector<Placement>> place;
    if (polygons.size() <= 0)
    {
        //add wangjx
        std::cout<<"\npolygons is null"<<std::endl;
        return place;
    }
    for(int i=0;i<polygons.size();i++)
    {
        //writeSegmentsTotxt(polygons[i],to_string(i)+"ori");
    }
    //for test
    std::vector<double> tmpMinMax=getMinMaxForSegments(*polygons[0].GetSegments());
    std::cout<<"tmpMinMax :"<<tmpMinMax[0]<<"\t"<<
               tmpMinMax[1]<<"\t"<<
               tmpMinMax[2]<<"\t"<<
               tmpMinMax[3]<<std::endl;
    CommonUtil::offsetTree(polygons,margin);
    for(int j=0;j<polygons.size();j++)
    {
        //writeSegmentsTotxt(polygons[j],to_string(j)+"Off");
    }
    Nest nest(bin, polygons, cfg, population);
    place = nest.startNest();
    if(place.size() != 0)
    {
        for (int i = 0; i < polygons.size(); i++)
        {
            for (int j = 0; j < place[0].size(); j++)
            {
                if (polygons.at(i).name == place[0].at(j).name)
                {
                    place[0].at(j).coorX = polygons.at(i).coor_x;
                    place[0].at(j).coorY = polygons.at(i).coor_y;
                    place[0].at(j).isTranslate = false;;
                }
            }
        }
    }
    return place;
}

void AutoPlace::translate(vector<NestPath> &binPath, vector<vector<Placement>> place)
{
    double TJ[3];
    double SX[3];
    double R[3][3];
    for (int i = 0; i < binPath.size(); i++)
    {
        for (int j = 0; j < place.size(); j++)
        {
            for (int n = 0; n < place[j].size(); n++)
            {
                if (binPath[i].bid == place[j][n].bid)
                {
                    R[0][0] = cos(place[j][n].rotate * pi / 180);
                    R[0][1] = sin(place[j][n].rotate * pi / 180);
                    R[0][2] = 0;
                    R[1][0] = -sin(place[j][n].rotate * pi / 180);
                    R[1][1] = cos(place[j][n].rotate * pi / 180);
                    R[1][2] = 0;
                    R[2][0] = place[j][n].translate.x;
                    R[2][1] = place[j][n].translate.y;
                    R[2][2] = 1;
                    break;
                }
            }

            for (int k = 0; k < binPath[i].segments->size(); k++)
            {
                TJ[0] = binPath[i].segments->at(k).x;
                TJ[1] = binPath[i].segments->at(k).y;
                TJ[2] = 1;
                for (int j = 0; j < 3; j++)
                {
                    SX[j] = TJ[0] * R[0][j] + TJ[1] * R[1][j] + TJ[2] * R[2][j];
                    if (SX[j] < -0.01)
                    {
                        int a = 0;
                    }
                }
                binPath[i].segments->at(k).x = SX[0];
                binPath[i].segments->at(k).y = SX[1];
            }
        }
    }
    return;
}

QFileInfoList AutoPlace::findFiles(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &suffixInfoList)
{
    if (strFilePath.isEmpty() || strNameFilters.isEmpty())
    {
        return suffixInfoList;
    }
    QFileInfoList infoList = QDir(strFilePath).entryInfoList();
    foreach(QFileInfo fileInfo, infoList)
    {
        if(!fileInfo.isFile()) continue;
        if(0==fileInfo.suffix().compare("txt"))
        {
            suffixInfoList << fileInfo;
        }
    }
    return suffixInfoList;
}
