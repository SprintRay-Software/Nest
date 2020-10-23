#pragma once


class ClipperCoor
{
public:
	ClipperCoor();
	ClipperCoor(long x, long y);
	long GetX();
	void SetX(long x);
	long GetY();
	void SetY(long y);

public:
	long m_x;
	long m_y;
};