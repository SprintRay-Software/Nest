#pragma once
#include "string"

using namespace std;

/**
 * @brief 里面的xy存放的是点的坐标
 *
 */
class Segments
{
public:
    bool equals(Segments obj);
	Segments();
	//Segments(Segments srcSeg);
	Segments(int x, int y);
	Segments(double x, double y);
    string toString();
	bool isMarked();
    void setMarked(bool marked1);
    Segments* getStart();
    void setStart(Segments *start);
    Segments* getEnd();
    void setEnd(Segments *end);
    double getX();
    void setX(double x);
    double getY();
    void setY(double y);
	Segments& operator=(const Segments srcSeg);
	bool operator!=(const Segments cls);
	bool operator==(const Segments cls);

public:
    double x;
    double y;
    bool marked = false;
    Segments *start;
    Segments *end;
};
