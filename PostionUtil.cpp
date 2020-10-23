#include "PostionUtil.h"

/**
 * ����һ��NestPath , ���ҽ�������任����ά����ƽ���ϡ�
 */


list<NestPath> PostionUtil::PositionTranslate4Path(double x, double y, list<NestPath> paths)
{
	for (NestPath path : paths) 
	{
		path.Translate(x, y);
		y = path.GetMaxY() + 10;
	}
	return paths;
}

