#pragma once


class SegmentRelation
{
public:
	SegmentRelation(int type, int a, int b);
	SegmentRelation();

public:
	int m_type;
	int m_a;
	int m_b;
};