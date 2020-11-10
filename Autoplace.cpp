#include <iostream>
#include<fstream>
#include<sstream>
#include "Autoplace.h"
#include "Nest.h"
#include "QString"
#include "QCoreApplication"
#include "QDir"
#include "QDirIterator"
#include "qDebug"
#include <QTextCodec>

using namespace std;

#define pi 3.14159265

bool Autoplace::read(string fileName, vector<NestPath> &polygons, double scale)
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
		while (count--) {
            string subStr;
            iss >> subStr;
			float x, y;
            sscanf(subStr.c_str(), "[%f,%f]", &x, &y);
            tempPolygon.Add(x, y);
		}
        string::size_type iPos = fileName.find_last_of('/') + 1;
        string tempFileName = fileName.substr(iPos, fileName.length() - iPos);
        string name = tempFileName.substr(0, tempFileName.rfind("."));
        tempPolygon.name = name;
        tempPolygon.setRotation(4);

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
        tempPolygon.coor_x = (maxX + minX) / 2;
        tempPolygon.coor_y = (maxY + minY) / 2;
        for (int i = 0; i < tempPolygon.segments->size(); i++)
		{
            tempPolygon.segments->at(i).x = (tempPolygon.segments->at(i).x - tempPolygon.coor_x) * scale + tempPolygon.coor_x;
            tempPolygon.segments->at(i).y = (tempPolygon.segments->at(i).y - tempPolygon.coor_y) * scale + tempPolygon.coor_y;
		}
        ClipperLib::ClipperOffset  clipperoff;
        ClipperLib::Paths dst_paths;
        ClipperLib::Path dst_path;
        for (int i = 0; i < tempPolygon.segments->size(); i++)
        {
            dst_path.push_back(ClipperLib::IntPoint(static_cast<ClipperLib::cInt>(tempPolygon.segments->at(i).x * 1000), static_cast<ClipperLib::cInt>(tempPolygon.segments->at(i).y * 1000)));
        }
        dst_paths.push_back(dst_path);
        clipperoff.AddPaths(dst_paths,ClipperLib::jtSquare,ClipperLib::etClosedPolygon);
        clipperoff.Execute(dst_paths,15 * 1000);
        tempPolygon.segments->clear();
        for (const auto& dst_path : dst_paths)
        {
            for (const auto& point : dst_path)
            {
                tempPolygon.segments->push_back(Segments((double)(point.X / 1000.0), (double)(point.Y / 1000.0)));
            }
        }
        polygons.push_back(tempPolygon);
	}
};

vector<vector<Placement>> Autoplace::translateCoor(double scale, double population, double length, double width, double offset, double para)
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
        strDir = QString(QString::fromLocal8Bit(str.c_str()));
        read(strDir.toStdString(), polygons, scale);
	}
	NestPath bin;
    double binWidth = length-2*offset/para;
    double binHeight = width-2*offset/para;
	bin.Add(0, 0);
	bin.Add(binWidth, 0);
	bin.Add(binWidth, binHeight);
	bin.Add(0, binHeight);
    bin.bid = -1;
	Config cfg;
    cfg.spacing = 0;
    cfg.populationSize = population;
	vector<vector<Placement>> place;
	if (polygons.size() <= 0)
	{
		return place;
	}
	Nest nest(bin, polygons, cfg, population);
    place = nest.startNest();

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
	return place;
}

void Autoplace::translate(vector<NestPath> &binPath, vector<vector<Placement>> place)
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

QFileInfoList Autoplace::findFiles(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &suffixInfoList)
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
