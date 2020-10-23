#pragma once
#include "string"

using namespace std;

class Segments
{
public:
	bool Equals(Segments obj);
	Segments();
	//Segments(Segments srcSeg);
	Segments(int x, int y);
	Segments(double x, double y);
	string ToString();
	bool isMarked();
	void SetMarked(bool marked1);
	Segments* GetStart();
	void SetStart(Segments *start);
	Segments* GetEnd();
	void SetEnd(Segments *end);
	double GetX();
	void SetX(double x);
	double GetY();
	void SetY(double y);
	Segments& operator=(const Segments srcSeg);
	bool operator!=(const Segments cls);
	bool operator==(const Segments cls);

public:
	double m_x;
	double m_y;

	bool m_marked = false;
	Segments *m_start;
	Segments *m_end;
};