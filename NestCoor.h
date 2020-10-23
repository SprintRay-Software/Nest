#pragma once

class NestCoor
{
public:
	NestCoor();
	NestCoor(double x, double y);
	double GetX();
	void SetX(double x);
	double GetY();
	void SetY(double y);

public:
	double m_x;
	double m_y;
};