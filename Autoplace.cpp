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

using namespace std;

#define pi 3.14159265

bool Autoplace::Read(string file_name, vector<NestPath> &temp_polygons, double scale)
{
	ifstream file;
	file.open(file_name, ios::in);
	if (!file.is_open()) return 0;
	string str;
    qDebug()<<"Read";
	while (getline(file, str))
	{
		int count;
		stringstream iss(str);
		iss >> count;
		NestPath polygon;
		while (count--) {
			string substr;
			iss >> substr;
			float x, y;
			sscanf(substr.c_str(), "[%f,%f]", &x, &y);
			polygon.Add(x, y);
			//polygon->bid = id;
		}
        string::size_type iPos = file_name.find_last_of('/') + 1;
        string filename = file_name.substr(iPos, file_name.length() - iPos);
        string name = filename.substr(0, filename.rfind("."));
        qDebug()<<"name = "<<QString::fromStdString(name);
		polygon.m_name = name;
		polygon.SetRotation(4);

		double min_x = polygon.m_segments->at(0).m_x, max_x = polygon.m_segments->at(0).m_x, min_y = polygon.m_segments->at(0).m_y, max_y = polygon.m_segments->at(0).m_y;
		for (int i = 0; i < polygon.m_segments->size(); i++)
		{
			if (polygon.m_segments->at(i).m_x <= min_x)
			{
				min_x = polygon.m_segments->at(i).m_x;
			}
			if (polygon.m_segments->at(i).m_x >= max_x)
			{
				max_x = polygon.m_segments->at(i).m_x;
			}
			if (polygon.m_segments->at(i).m_y <= min_y)
			{
				min_y = polygon.m_segments->at(i).m_y;
			}
			if (polygon.m_segments->at(i).m_y >= max_y)
			{
				max_y = polygon.m_segments->at(i).m_y;
			}
		}
		polygon.m_coor_x = (max_x + min_x) / 2;
		polygon.m_coor_y = (max_y + min_y) / 2;
		for (int i = 0; i < polygon.m_segments->size(); i++)
		{
			polygon.m_segments->at(i).m_x = (polygon.m_segments->at(i).m_x - polygon.m_coor_x) * scale + polygon.m_coor_x;
			polygon.m_segments->at(i).m_y = (polygon.m_segments->at(i).m_y - polygon.m_coor_y) * scale + polygon.m_coor_y;
		}
		temp_polygons.push_back(polygon);
	}
};

//string Autoplace::Print_Path()
//{
//	TCHAR Buffer[1000];
//	string a;
//	DWORD dwRet;
//	GetModuleFileName(NULL, (LPSTR)Buffer, sizeof(Buffer));
//	string path = Buffer;
//	int pos = path.rfind("\\", path.length());
//	path = path.substr(0, pos);
//	path += "\\Borderfinder\\";
//	return path;
//}

vector<vector<Placement>> Autoplace::Translate_Coor(double scale, double population)
{
	vector<NestPath> polygons;

	//GetModuleFileName(NULL, (LPSTR)Buffer, sizeof(Buffer));
	//string path = Buffer;
	//int pos = path.rfind("\\", path.length());
	//path = path.substr(0, pos);

    QString path = QCoreApplication::applicationDirPath() + "/Borderfinder/";
    QFileInfoList filelist;
    qDebug()<< "path:"<<path;
    Find_Files(path, ".txt",filelist);

    qDebug()<< "filelist.size:"<<filelist.size();

	for (int i = 0; i < filelist.size(); i++)
	{
        qDebug()<< "filelist.name:"<<filelist.at(i).fileName();
        Read((path.toStdString() + filelist.at(i).fileName().toStdString()), polygons, scale);
	}


	NestPath bin;
	double binWidth = 1920;
	double binHeight = 1080;
	bin.Add(0, 0);
	bin.Add(binWidth, 0);
	bin.Add(binWidth, binHeight);
	bin.Add(0, binHeight);
	bin.m_bid = -1;

	Config cfg;
	cfg.m_spacing = 0;
	cfg.m_population_size = population;

	vector<vector<Placement>> place;
    qDebug()<< "polygons.size:"<<polygons.size();
	if (polygons.size() <= 0)
	{
		return place;
	}
	Nest nest(bin, polygons, cfg, population);
	place = nest.StartNest();                 

	for (int i = 0; i < polygons.size(); i++)
	{
		for (int j = 0; j < place[0].size(); j++)
		{
			if (polygons.at(i).m_name == place[0].at(j).m_name)
			{
				place[0].at(j).m_coor_x = polygons.at(i).m_coor_x;
				place[0].at(j).m_coor_y = polygons.at(i).m_coor_y;
				place[0].at(j).m_istranslate = false;;
			}
		}
	}

	//Translate(*polygons, place);
	return place;
}

void Autoplace::Translate(vector<NestPath> &binPath_para, vector<vector<Placement>> place)
{
	double TJ[3];
	double SX[3];
	double R[3][3];
	for (int i = 0; i < binPath_para.size(); i++)
	{
		for (int j = 0; j < place.size(); j++)
		{
			//double R[3][3] = { {cos(place[j][i].rotate), sin(place[j][i].rotate), place[j][i].translate.x * cos(place[j][i].rotate)+ place[j][i].translate.y * sin(place[j][i].rotate)} ,
			//{-sin(place[j][i].rotate), cos(place[j][i].rotate), -place[j][i].translate.x * sin(place[j][i].rotate) + place[j][i].translate.y * cos(place[j][i].rotate)},
			//{0, 0, 1} };
			for (int n = 0; n < place[j].size(); n++)
			{
				if (binPath_para[i].m_bid == place[j][n].m_bid)
				{
//					double R[3][3] = { {cos(place[j][n].rotate * pi / 180), sin(place[j][n].rotate * pi / 180), 0} ,
//{-sin(place[j][n].rotate * pi / 180), cos(place[j][n].rotate * pi / 180), 0},
//{place[j][n].translate.x, place[j][n].translate.y, 1} };
					R[0][0] = cos(place[j][n].m_rotate * pi / 180);
					R[0][1] = sin(place[j][n].m_rotate * pi / 180);
					R[0][2] = 0;
					R[1][0] = -sin(place[j][n].m_rotate * pi / 180);
					R[1][1] = cos(place[j][n].m_rotate * pi / 180);
					R[1][2] = 0;
					R[2][0] = place[j][n].m_translate.m_x;
					R[2][1] = place[j][n].m_translate.m_y;
					R[2][2] = 1;
					break;
				}
			}

			for (int k = 0; k < binPath_para[i].m_segments->size(); k++)
			{
				TJ[0] = binPath_para[i].m_segments->at(k).m_x;
				TJ[1] = binPath_para[i].m_segments->at(k).m_y;
				TJ[2] = 1;
				for (int j = 0; j < 3; j++)    
				{
					SX[j] = TJ[0] * R[0][j] + TJ[1] * R[1][j] + TJ[2] * R[2][j];
					if (SX[j] < -0.01)
					{
						int a = 0;
					}
				}
				binPath_para[i].m_segments->at(k).m_x = SX[0];
				binPath_para[i].m_segments->at(k).m_y = SX[1];
			}
		}
	}
	return;
}

QFileInfoList Autoplace::Find_Files(const QString &strFilePath, const QString &strNameFilters, QFileInfoList &SuffixInfoList)
{
    if (strFilePath.isEmpty() || strNameFilters.isEmpty())
    {
        return SuffixInfoList;
    }
    QFileInfoList InfoList = QDir(strFilePath).entryInfoList();
    qDebug()<< "InfoList.size:"<<InfoList.size();
    foreach(QFileInfo fileInfo, InfoList)
    {
        if(!fileInfo.isFile()) continue;
        qDebug()<< "fileInfo.suffix:"<<fileInfo.suffix();
        if(0==fileInfo.suffix().compare("txt"))
        {
            qDebug()<< "SuffixInfoList.size:"<<SuffixInfoList.size();
            qDebug()<< "SuffixInfoList << fileInfo";
            SuffixInfoList << fileInfo;
            qDebug()<< "SuffixInfoList.size2:"<<SuffixInfoList.size();
        }
    }
    return SuffixInfoList;
}
