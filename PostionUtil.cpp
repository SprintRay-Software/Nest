#include "PostionUtil.h"

/**
 * 接收一组NestPath , 并且将其坐标变换到二维坐标平面上。
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

