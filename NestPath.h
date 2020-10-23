#pragma once
#include <iostream>
#include "string"
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
	bool Equals(NestPath obj);
	Config* GetConfig();
	void SetConfig(Config *config);
	void Pop();
	void Reverse();
	Segments& Get(int i);
	NestPath GetParent();
	void SetParent(NestPath *parent);
	void AddChildren(NestPath nestPath);
	string ToString();
	vector<NestPath>* GetChildren();
	void SetChildren(vector<NestPath> *children);
	int GetRotation();
	void SetRotation(int rotation_para);
	void SetSegments(vector<Segments> *segments);
	int GetSource();
	void SetSource(int source);
	static NestPath CleanNestPath(NestPath srcPath);
	void Zerolize();
	void Clear();
	int Size();
	void Add(Segments s);
	vector<Segments>* GetSegments();
	int GetId();
	void SetId(int id_para);
	double GetOffsetX();
	void SetOffsetX(double offsetX);
	double GetOffsetY();
	void SetOffsetY(double offsetY);
	int CompareTo(NestPath o);
	double GetMaxY();
	void Translate(double x, double y);
	bool operator==(const NestPath& np);
	bool operator < (const NestPath& rhs);

private:
	void ZeroX();
	void ZeroY();

public:
	double m_offsetX;
	double m_offsetY;
	Config *m_config;
	double m_area;
	int m_bid;
	vector<Segments> *m_segments;
	string m_name;
	double m_coor_x;
	double m_coor_y;

private:

	vector<NestPath> *m_children;
	NestPath *m_parent;
	int m_id;
	int m_source;
	int m_rotation;
};
