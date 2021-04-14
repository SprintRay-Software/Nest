#include "SvgUtil.h"

vector<string> SvgUtil::svgGenerator(vector<NestPath> list, vector<vector<Placement>> applied, double binWidth, double binHeight)
{
	vector<string> strings;
	int x = 10;
	int y = 0;
	for (int i = 0; i < applied.size(); i++)
	{
		string s = " <g transform=\"translate(" + to_string(x) + "  " + to_string(y) + ")\">" + "\n";
        s += "    <rect x=\"0\" y=\"0\" width=\"" + to_string(binWidth) + "\" height=\"" + to_string(binHeight) + "\"  fill=\"none\" stroke=\"#010101\" stroke-width=\"1\" />\n";
		for (int j = 0; j < applied.size(); j++)
		{
			Placement placement = applied.at(i).at(j);
            int bid = placement.bid;
            NestPath nestPath = getNestPathByBid(bid, list);
            double ox = placement.translate.getX();
            double oy = placement.translate.getY();
            double rotate = placement.rotate;
			s += "<g transform=\"translate(" + to_string(ox + x) + " " + to_string(oy + y) + ") rotate(" + to_string(rotate) + ")\"> \n";
			s += "<path d=\"";
			for (int i = 0; i < nestPath.GetSegments()->size(); i++) 
			{
				if (i == 0) 
				{
					s += "M";
				}
				else 
				{
					s += "L";
				}
				Segments Segments = nestPath.Get(i);
                s += to_string(Segments.getX()) + " " + to_string(Segments.y) + " ";
			}
			s += "Z\" fill=\"#8498d1\" stroke=\"#010101\" stroke-width=\"1\" /> \n";
			s += "</g> \n";
		}
		s += "</g> \n";
		y += binHeight + 50;
		strings.push_back(s);
	}
	return strings;
}

NestPath SvgUtil::getNestPathByBid(int bid, vector<NestPath> list)
{
	NestPath nestPath;
	for (int i = 0; i < list.size(); i++)
	{
		nestPath = list.at(i);
        if (nestPath.bid == bid)
		{
			return nestPath;
		}
	}
	return nestPath;
}

