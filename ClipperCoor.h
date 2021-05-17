#pragma once


class ClipperCoor
{
public:
	ClipperCoor();
	ClipperCoor(long x, long y);
    long getX();
    void setX(long x);
    long getY();
    void setY(long y);

public:
    long x;
    long y;
};
