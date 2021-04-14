#include "PostionUtil.h"


list<NestPath> PostionUtil::positionTranslate4Path(double x, double y, list<NestPath> paths)
{
	for (NestPath path : paths) 
	{
        path.translate(x, y);
        y = path.getMaxY() + 10;
	}
	return paths;
}

