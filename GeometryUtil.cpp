#include "GeometryUtil.h"
#define PI 3.1415926
/**
 *      y
 *      ↑
 *      |
 *      |
 *      |__________ x 项目的坐标系
 *
 */
vector<NestPath> *nfpRect;

double GeometryUtil::m_tol = pow(10, -2);

bool GeometryUtil::AlmostEqual(double a, double b)
{
	return abs(a - b) < m_tol;
}

bool GeometryUtil::AlmostEqual(double a, double b, double tolerance)
{
	return abs(a - b) < tolerance;
}

/**
	* 计算多边形面积
	* @param polygon
	* @return
	*/
double GeometryUtil::PolygonArea(NestPath polygon)
{
	double area = 0;
	for (int i = 0, j = polygon.Size() - 1; i < polygon.Size(); j = i++) 
	{
		Segments si = polygon.GetSegments()->at(i);
		Segments sj = polygon.GetSegments()->at(j);
		area += (sj.GetX() + si.GetX()) * (sj.GetY() - si.GetY());
	}
	return 0.5*area;
}

/**
	* 判断点P是否在边AB上
	* @param A
	* @param B
	* @param p
	* @return
	*/
bool GeometryUtil::OnSegment(Segments A, Segments B, Segments p)
{
	// vertical line
	if (AlmostEqual(A.m_x, B.m_x) && AlmostEqual(p.m_x, A.m_x))
	{
		if (!AlmostEqual(p.m_y, B.m_y) && !AlmostEqual(p.m_y, A.m_y) && p.m_y < max(B.m_y, A.m_y) && p.m_y > min(B.m_y, A.m_y))
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	// horizontal line
	if (AlmostEqual(A.m_y, B.m_y) && AlmostEqual(p.m_y, A.m_y))
	{
		if (!AlmostEqual(p.m_x, B.m_x) && !AlmostEqual(p.m_x, A.m_x) && p.m_x < max(B.m_x, A.m_x) && p.m_x > min(B.m_x, A.m_x))
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	//range check
	if ((p.m_x < A.m_x && p.m_x < B.m_x) || (p.m_x > A.m_x && p.m_x > B.m_x) || (p.m_y < A.m_y && p.m_y < B.m_y) || (p.m_y > A.m_y && p.m_y > B.m_y))
	{
		return false;
	}


	// exclude end points
	if ((AlmostEqual(p.m_x, A.m_x) && AlmostEqual(p.m_y, A.m_y)) || (AlmostEqual(p.m_x, B.m_x) && AlmostEqual(p.m_y, B.m_y)))
	{
		return false;
	}

	double cross = (p.m_y - A.m_y) * (B.m_x - A.m_x) - (p.m_x - A.m_x) * (B.m_y - A.m_y);

	if (abs(cross) > m_tol)
	{
		return false;
	}

	double dot = (p.m_x - A.m_x) * (B.m_x - A.m_x) + (p.m_y - A.m_y)*(B.m_y - A.m_y);



	if (dot < 0 || AlmostEqual(dot, 0)) 
	{
		return false;
	}

	double len2 = (B.m_x - A.m_x)*(B.m_x - A.m_x) + (B.m_y - A.m_y)*(B.m_y - A.m_y);



	if (dot > len2 || AlmostEqual(dot, len2)) 
	{
		return false;
	}

	return true;

}

/**
	* 判断点P是否在多边形polygon上
	* @param point
	* @param polygon
	* @return
	*/
bool GeometryUtil::PointInPolygon(Segments point, NestPath polygon)
{
	bool inside = false;
	double offsetx = polygon.m_offsetX;
	double offsety = polygon.m_offsetY;

	for (int i = 0, j = polygon.Size() - 1; i < polygon.Size(); j = i++) 
	{
		double xi = polygon.Get(i).m_x + offsetx;
		double yi = polygon.Get(i).m_y + offsety;
		double xj = polygon.Get(j).m_x + offsetx;
		double yj = polygon.Get(j).m_y + offsety;

		if (AlmostEqual(xi, point.m_x) && AlmostEqual(yi, point.m_y))
		{
			return inside; // no result
		}

		if (OnSegment(Segments(xi, yi), Segments(xj, yj), point)) 
		{
			return inside; // exactly on the Segments
		}

		if (AlmostEqual(xi, xj) && AlmostEqual(yi, yj)) { // ignore very small lines
			continue;
		}

		bool intersect = ((yi > point.m_y) != (yj > point.m_y)) && (point.m_x < (xj - xi) * (point.m_y - yi) / (yj - yi) + xi);
		if (intersect)
		{
			inside = !inside;
		}
	}

	return inside;
}

/**
	* 获取多边形边界
	* @param polygon
	* @return
	*/
Bound GeometryUtil::GetPolygonBounds(NestPath polygon)
{

	double xmin = polygon.GetSegments()->at(0).GetX();
	double xmax = polygon.GetSegments()->at(0).GetX();
	double ymin = polygon.GetSegments()->at(0).GetY();
	double ymax = polygon.GetSegments()->at(0).GetY();

	for (int i = 1; i < polygon.GetSegments()->size(); i++)
	{
		double x = polygon.GetSegments()->at(i).GetX();
		double y = polygon.GetSegments()->at(i).GetY();
		if (x > xmax) 
		{
			xmax = x;
		}
		else if (x < xmin) 
		{
			xmin = x;
		}

		if (y > ymax) 
		{
			ymax = y;
		}
		else if (y < ymin) 
		{
			ymin = y;
		}
	}
	return Bound(xmin, ymin, xmax - xmin, ymax - ymin);
}

/**
	* 将多边形旋转一定角度后，返回旋转后多边形的边界
	* @param polygon
	* @param angle
	* @return
	*/
Bound GeometryUtil::RotatePolygon(NestPath polygon, int angle)
{
	if (angle == 0) 
	{
		return GetPolygonBounds(polygon);
	}
	double Fangle = angle * PI / 180;
	NestPath rotated;
	for (int i = 0; i < polygon.Size(); i++) 
	{
		double x = polygon.Get(i).m_x;
		double y = polygon.Get(i).m_y;
		double x1 = x * cos(Fangle) - y * sin(Fangle);
		double y1 = x * sin(Fangle) + y * cos(Fangle);
		rotated.Add(x1, y1);
	}
	Bound bounds = GetPolygonBounds(rotated);
	return bounds;
}

/**
	* 将多边形旋转一定角度后，返回该旋转后的多边形
	* @param polygon
	* @param degrees
	* @return
	*/
NestPath GeometryUtil::RotatePolygon2Polygon(NestPath polygon, int degrees)
{
	NestPath rotated;
	double angle = degrees * PI / 180;
	for (int i = 0; i < polygon.Size(); i++) 
	{
		double x = polygon.Get(i).m_x;
		double y = polygon.Get(i).m_y;
		double x1 = x * cos(angle) - y * sin(angle);
		double y1 = x * sin(angle) + y * cos(angle);
		rotated.Add(Segments(x1, y1));
	}
	rotated.m_bid = polygon.m_bid;
	rotated.SetId(polygon.GetId());
	rotated.SetSource(polygon.GetSource());
	if (polygon.GetChildren()->size() > 0)
	{
		for (int j = 0; j < polygon.GetChildren()->size(); j++)
		{
			rotated.GetChildren()->push_back(RotatePolygon2Polygon(polygon.GetChildren()->at(j), degrees));
		}
	}
	return rotated;
}

/**
	* 判断是否是矩形
	* @param poly
	* @param tolerance
	* @return
	*/
bool GeometryUtil::isRectangle(NestPath poly, double tolerance)
{
	Bound bb = GetPolygonBounds(poly);

	for (int i = 0; i < poly.Size(); i++) 
	{
		if (!AlmostEqual(poly.Get(i).m_x, bb.GetXmin(), tolerance) && !AlmostEqual(poly.Get(i).m_x, bb.GetXmin() + bb.GetWidth(), tolerance))
		{
			return false;
		}
		if (!AlmostEqual(poly.Get(i).m_y, bb.GetYmin(), tolerance) && !AlmostEqual(poly.Get(i).m_y, bb.GetYmin() + bb.GetHeight(), tolerance))
		{
			return false;
		}
	}
	return true;
}

/**
	* 构建NFP
	* @param A
	* @param B
	* @param inside
	* @param searchEdges
	* @return
	*/
vector<NestPath> GeometryUtil::NoFitPolygon(NestPath A, NestPath B, bool inside, bool searchEdges)
{
	A.SetOffsetX(0);
	A.SetOffsetY(0);

	double minA = A.Get(0).m_y;
	int minAIndex = 0;
	double currentAX = A.Get(0).m_x;
	double maxB = B.Get(0).m_y;
	int maxBIndex = 0;

	for (int i = 1; i < A.Size(); i++) 
	{
		A.Get(i).m_marked = false;
		
		if (AlmostEqual(A.Get(i).m_y, minA) && A.Get(i).m_x < currentAX)
		{
			minA = A.Get(i).m_y;
			minAIndex = i;
			currentAX = A.Get(i).m_x;
		}
		else if (A.Get(i).m_y < minA)
		{
			minA = A.Get(i).m_y;
			minAIndex = i;
			currentAX = A.Get(i).m_x;
		}
	}
	for (int i = 1; i < B.Size(); i++) 
	{
		B.Get(i).m_marked = false;
		if (B.Get(i).m_y > maxB)
		{
			maxB = B.Get(i).m_y;
			maxBIndex = i;
		}
	}
	Segments *startPoint = NULL;
	if (!inside) 
	{
		startPoint = new Segments(A.Get(minAIndex).m_x - B.Get(maxBIndex).m_x,
			A.Get(minAIndex).m_y - B.Get(maxBIndex).m_y);

	}
	else 
	{
		//TODO heuristic for inside
		vector<NestPath> temp_vec;
		startPoint = SearchStartPoint(A, B, true, temp_vec);

	}

	vector<NestPath> *NFPlist = new vector<NestPath>();

	while (startPoint != NULL)
	{
		Segments *prevvector = NULL;
		B.SetOffsetX(startPoint->m_x);
		B.SetOffsetY(startPoint->m_y);


		vector<SegmentRelation> *touching;
		NestPath* NFP = new NestPath();
		NFP->Add(Segments(B.Get(0).m_x + B.GetOffsetX(),
			B.Get(0).m_y + B.GetOffsetY()));

		double referenceX = B.Get(0).m_x + B.GetOffsetX();
		double referenceY = B.Get(0).m_y + B.GetOffsetY();
		double startX = referenceX;
		double startY = referenceY;
		int counter = 0;

		// sanity check  , prevent infinite loop
		while (counter < 10 * (A.Size() + B.Size())) 
		{
			touching = new vector<SegmentRelation>();


			for (int i = 0; i < A.Size(); i++) 
			{
				int nexti = (i == A.Size() - 1) ? 0 : i + 1;
				for (int j = 0; j < B.Size(); j++) {
					int nextj = (j == B.Size() - 1) ? 0 : j + 1;
					if (AlmostEqual(A.Get(i).m_x, B.Get(j).m_x + B.m_offsetX) && AlmostEqual(A.Get(i).m_y, B.Get(j).m_y + B.m_offsetY))
					{
						touching->push_back(SegmentRelation(0, i, j));
					}
					else if (OnSegment(A.Get(i), A.Get(nexti), Segments(B.Get(j).m_x + B.m_offsetX, B.Get(j).m_y + B.m_offsetY)))
					{
						touching->push_back(SegmentRelation(1, nexti, j));
					}
					else if (OnSegment(Segments(B.Get(j).m_x + B.m_offsetX, B.Get(j).m_y + B.m_offsetY),
						Segments(B.Get(nextj).m_x + B.m_offsetX, B.Get(nextj).m_y + B.m_offsetY),
						A.Get(i))) 
					{
						touching->push_back(SegmentRelation(2, i, nextj));
					}
				}
			}


			NestPath *vectors = new NestPath();
			for (int i = 0; i < touching->size(); i++)
			{
				Segments vertexA = A.Get(touching->at(i).m_a);
				vertexA.m_marked = true;

				int prevAIndex = touching->at(i).m_a - 1;
				int nextAIndex = touching->at(i).m_a + 1;

				prevAIndex = (prevAIndex < 0) ? A.Size() - 1 : prevAIndex; // loop
				nextAIndex = (nextAIndex >= A.Size()) ? 0 : nextAIndex; // loop

				Segments prevA = A.Get(prevAIndex);
				Segments nextA = A.Get(nextAIndex);

				Segments vertexB = B.Get(touching->at(i).m_b);

				int prevBIndex = touching->at(i).m_b - 1;
				int nextBIndex = touching->at(i).m_b + 1;

				prevBIndex = (prevBIndex < 0) ? B.Size() - 1 : prevBIndex; // loop
				nextBIndex = (nextBIndex >= B.Size()) ? 0 : nextBIndex; // loop

				Segments prevB = B.Get(prevBIndex);
				Segments nextB = B.Get(nextBIndex);

				if (touching->at(i).m_type == 0) {
					Segments vA1(prevA.m_x - vertexA.m_x, prevA.m_y - vertexA.m_y);
					vA1.m_start = &vertexA; vA1.m_end = &prevA;

					Segments vA2(nextA.m_x - vertexA.m_x, nextA.m_y - vertexA.m_y);
					vA2.m_start = &vertexA; vA2.m_end = &nextA;

					Segments vB1(vertexB.m_x - prevB.m_x, vertexB.m_y - prevB.m_y);
					vB1.m_start = &prevB; vB1.m_end = &vertexB;

					Segments vB2(vertexB.m_x - nextB.m_x, vertexB.m_y - nextB.m_y);
					vB2.m_start = &nextB; vB2.m_end = &vertexB;

					vectors->Add(vA1);
					vectors->Add(vA2);
					vectors->Add(vB1);
					vectors->Add(vB2);
				}
				else if (touching->at(i).m_type == 1)
				{

					Segments tmp(vertexA.m_x - (vertexB.m_x + B.m_offsetX),
						vertexA.m_y - (vertexB.m_y + B.m_offsetY));

					tmp.m_start = &prevA;
					tmp.m_end = &vertexA;

					Segments tmp2(prevA.m_x - (vertexB.m_x + B.m_offsetX), prevA.m_y - (vertexB.m_y + B.m_offsetY));
					tmp2.m_start = &vertexA; tmp2.m_end = &prevA;
					vectors->Add(tmp);
					vectors->Add(tmp2);

				}
				else if (touching->at(i).m_type == 2)
				{
					Segments tmp1(vertexA.m_x - (vertexB.m_x + B.m_offsetX),
						vertexA.m_y - (vertexB.m_y + B.m_offsetY));
					tmp1.m_start = &prevB;
					tmp1.m_end = &vertexB;
					Segments tmp2(vertexA.m_x - (prevB.m_x + B.m_offsetX),
						vertexA.m_y - (prevB.m_y + B.m_offsetY));
					tmp2.m_start = &vertexB;
					tmp2.m_end = &prevB;

					vectors->Add(tmp1); vectors->Add(tmp2);
				}
			}

			Segments *translate = NULL;

			double maxd = 0.0;
			for (int i = 0; i < vectors->Size(); i++)
			{
				if (AlmostEqual(vectors->Get(i).m_x, 0) && AlmostEqual(vectors->Get(i).m_y, 0))
				{
					continue;
				}

				if (prevvector != NULL && vectors->Get(i).m_y * prevvector->m_y + vectors->Get(i).m_x * prevvector->m_x < 0)
				{

					double vectorlength = sqrt(vectors->Get(i).m_x*vectors->Get(i).m_x + vectors->Get(i).m_y*vectors->Get(i).m_y);
					Segments unitv(vectors->Get(i).m_x / vectorlength, vectors->Get(i).m_y / vectorlength);


					double prevlength = sqrt(prevvector->m_x*prevvector->m_x + prevvector->m_y*prevvector->m_y);
					Segments prevunit(prevvector->m_x / prevlength, prevvector->m_y / prevlength);


					// we need to scale down to unit vectors to normalize vector length. Could also just do a tan here
					if (abs(unitv.m_y * prevunit.m_x - unitv.m_x * prevunit.m_y) < 0.0001)
					{

						continue;
					}
				}
				//todo polygonSlideDistance
				double d = PolygonSlideDistance(A, B, vectors->Get(i), true);

				double vecd2 = vectors->Get(i).m_x*vectors->Get(i).m_x + vectors->Get(i).m_y*vectors->Get(i).m_y;

				if (d == -1 || d * d > vecd2) 
				{
					double vecd = sqrt(vectors->Get(i).m_x*vectors->Get(i).m_x + vectors->Get(i).m_y*vectors->Get(i).m_y);
					d = vecd;
				}

				if (d != -1 && d > maxd) 
				{
					maxd = d;
					translate = &(vectors->Get(i));
				}

			}

			if (translate == NULL || AlmostEqual(maxd, 0)) 
			{
				// didn't close the loop, something went wrong here
				if (translate == NULL)
				{

				}
				if (AlmostEqual(maxd, 0)) 
				{
				}
				NFP = NULL;
				break;
			}

			translate->m_start->m_marked = true;
			translate->m_end->m_marked = true;

			prevvector = translate;


			// trim
			double vlength2 = translate->m_x*translate->m_x + translate->m_y*translate->m_y;
			if (maxd*maxd < vlength2 && !AlmostEqual(maxd*maxd, vlength2)) 
			{
				double scale = sqrt((maxd*maxd) / vlength2);
				translate->m_x *= scale;
				translate->m_y *= scale;
			}

			referenceX += translate->m_x;
			referenceY += translate->m_y;


			if (AlmostEqual(referenceX, startX) && AlmostEqual(referenceY, startY)) 
			{
				// we've made a full loop
				break;
			}

			// if A and B start on a touching horizontal line, the end point may not be the start point
			bool looped = false;
			if (NFP->Size() > 0) 
			{
				for (int i = 0; i < NFP->Size() - 1; i++)
				{
					if (AlmostEqual(referenceX, NFP->Get(i).m_x) && AlmostEqual(referenceY, NFP->Get(i).m_y))
					{
						looped = true;
					}
				}
			}

			if (looped) 
			{
				// we've made a full loop
				break;
			}

			NFP->Add(Segments(referenceX, referenceY));

			B.m_offsetX += translate->m_x;
			B.m_offsetY += translate->m_y;
			counter++;
		}

		if (NFP != NULL && NFP->Size() > 0) 
		{
			NFPlist->push_back(*NFP);
		}

		if (!searchEdges) 
		{
			// only get outer NFP or first inner NFP
			break;
		}
		startPoint = SearchStartPoint(A, B, inside, *NFPlist);
	}
	return *NFPlist;
}

Segments* GeometryUtil::SearchStartPoint(NestPath CA, NestPath CB, bool inside, vector<NestPath> NFP) 
{

	NestPath A(CA);
	NestPath B(CB);

	if (A.Get(0) != A.Get(A.Size() - 1))
	{
		A.Add(A.Get(0));
	}

	if (B.Get(0) != B.Get(B.Size() - 1))
	{
		B.Add(B.Get(0));
	}

	for (int i = 0; i < A.Size() - 1; i++)
	{
		if (!A.Get(i).m_marked)
		{
			A.Get(i).m_marked = true;
			for (int j = 0; j < B.Size(); j++)
			{
				B.m_offsetX = A.Get(i).m_x - B.Get(j).m_x;
				B.m_offsetY = A.Get(i).m_y - B.Get(j).m_y;
				bool Binside = false;
				for (int k = 0; k < B.Size(); k++)
				{
					bool inpoly = PointInPolygon(Segments(B.Get(k).m_x + B.m_offsetX, B.Get(k).m_y + B.m_offsetY), A);
					//if (inpoly != null) 
					{
						Binside = inpoly;
						break;
					}
				}

				if (Binside == false) 
				{ // A and B are the same
					return NULL;
				}

				Segments* startPoint = new Segments(B.m_offsetX, B.m_offsetY);

				if (((Binside != false && inside) || (Binside == false && !inside)) && !Intersect(A, B) && !InNfp(*startPoint, NFP))
				{
					return startPoint;
				}

				// slide B along vector
				double vx = A.Get(i + 1).m_x - A.Get(i).m_x;
				double vy = A.Get(i + 1).m_y - A.Get(i).m_y;

				double d1 = PolygonProjectionDistance(A, B, Segments(vx, vy));
				double d2 = PolygonProjectionDistance(B, A, Segments(-vx, -vy));

				double d = -1;

				// todo: clean this up
				if (d1 == -1 && d2 == -1) 
				{
					// nothin
				}
				else if (d1 == -1) 
				{
					d = d2;
				}
				else if (d2 == -1) 
				{
					d = d1;
				}
				else 
				{
					d = min(d1, d2);
				}

				// only slide until no longer negative
				// todo: clean this up
				if (d != -1 && !AlmostEqual(d, 0) && d > 0) 
				{

				}
				else 
				{
					continue;
				}

				double vd2 = vx * vx + vy * vy;

				if (d*d < vd2 && !AlmostEqual(d*d, vd2)) 
				{
					double vd = sqrt(vx*vx + vy * vy);
					vx *= d / vd;
					vy *= d / vd;
				}

				B.m_offsetX += vx;
				B.m_offsetY += vy;

				for (int k = 0; k < B.Size(); k++)
				{
					bool inpoly = PointInPolygon(Segments(B.Get(k).m_x + B.m_offsetX, B.Get(k).m_y + B.m_offsetY), A);
					//if (inpoly != null) 
					{
						Binside = inpoly;
						break;
					}
				}
				startPoint = new Segments(B.m_offsetX, B.m_offsetY);
				if (((Binside && inside) || (!Binside && !inside)) && !Intersect(A, B) && !InNfp(*startPoint, NFP)) 
				{
					return startPoint;
				}
			}
		}
	}
	return NULL;
}


/**
	*
	* @param p
	* @param nfp
	* @return
	*/
bool GeometryUtil::InNfp(Segments p, vector<NestPath>& nfp)
{
	if (nfp.empty()) 
	{
		return false;
	}
	for (int i = 0; i < nfp.size(); i++) 
	{
		for (int j = 0; j < nfp.at(i).Size(); j++)
		{
			if (AlmostEqual(p.m_x, nfp.at(i).Get(j).m_x) && AlmostEqual(p.m_y, nfp.at(i).Get(j).m_y))
			{
				return true;
			}
		}
	}
	return false;
}

double GeometryUtil::PolygonProjectionDistance(NestPath CA, NestPath CB, Segments direction) 
{
	double Boffsetx = CB.m_offsetX;
	double Boffsety = CB.m_offsetY;

	double Aoffsetx = CA.m_offsetX;
	double Aoffsety = CA.m_offsetY;

	NestPath A(CA);
	NestPath B(CB);

	if (A.Get(0) != A.Get(A.Size() - 1))
	{
		A.Add(A.Get(0));
	}

	if (B.Get(0) != B.Get(B.Size() - 1))
	{
		B.Add(B.Get(0));
	}

	NestPath edgeA = A;
	NestPath edgeB = B;

	double distance = -1;
	Segments p, s1, s2;
	double d = -1;
	for (int i = 0; i < edgeB.Size(); i++) 
	{
		// the shortest/most negative projection of B onto A
		double minprojection = -1;
		Segments minp;
		for (int j = 0; j < edgeA.Size() - 1; j++) 
		{
			p = Segments(edgeB.Get(i).m_x + Boffsetx, edgeB.Get(i).m_y + Boffsety);
			s1 = Segments(edgeA.Get(j).m_x + Aoffsetx, edgeA.Get(j).m_y + Aoffsety);
			s2 = Segments(edgeA.Get(j + 1).m_x + Aoffsetx, edgeA.Get(j + 1).m_y + Aoffsety);
			if (abs((s2.m_y - s1.m_y) * direction.m_x - (s2.m_x - s1.m_x) * direction.m_y) < m_tol)
			{
				continue;
			}

			// project point, ignore edge boundaries
			d = PointDistance(p, s1, s2, direction, false);

			if (d != -1 && (minprojection == -1 || d < minprojection)) 
			{
				minprojection = d;
				minp = p;
			}
		}
		if (minprojection != -1 && (distance == -1 || minprojection > distance)) 
		{
			distance = minprojection;
		}
	}
	return distance;
}

bool GeometryUtil::Intersect(NestPath CA, NestPath CB) 
{
	double Aoffsetx = CA.m_offsetX;
	double Aoffsety = CA.m_offsetY;

	double Boffsetx = CB.m_offsetX;
	double Boffsety = CB.m_offsetY;

	NestPath A(CA);
	NestPath B(CB);

	for (int i = 0; i < A.Size() - 1; i++)
	{
		for (int j = 0; j < B.Size() - 1; j++)
		{
			Segments a1(A.Get(i).m_x + Aoffsetx, A.Get(i).m_y + Aoffsety);
			Segments a2(A.Get(i + 1).m_x + Aoffsetx, A.Get(i + 1).m_y + Aoffsety);
			Segments b1(B.Get(j).m_x + Boffsetx, B.Get(j).m_y + Boffsety);
			Segments b2(B.Get(j + 1).m_x + Boffsetx, B.Get(j + 1).m_y + Boffsety);


			int prevbindex = (j == 0) ? B.Size() - 1 : j - 1;
			int prevaindex = (i == 0) ? A.Size() - 1 : i - 1;
			int nextbindex = (j + 1 == B.Size() - 1) ? 0 : j + 2;
			int nextaindex = (i + 1 == A.Size() - 1) ? 0 : i + 2;

			// go even further back if we happen to hit on a loop end point
			if (B.Get(prevbindex) == B.Get(j) || (AlmostEqual(B.Get(prevbindex).m_x, B.Get(j).m_x) && AlmostEqual(B.Get(prevbindex).m_y, B.Get(j).m_y)))
			{
				prevbindex = (prevbindex == 0) ? B.Size() - 1 : prevbindex - 1;
			}

			if (A.Get(prevaindex) == A.Get(i) || (AlmostEqual(A.Get(prevaindex).m_x, A.Get(i).m_x) && AlmostEqual(A.Get(prevaindex).m_y, A.Get(i).m_y)))
			{
				prevaindex = (prevaindex == 0) ? A.Size() - 1 : prevaindex - 1;
			}

			// go even further forward if we happen to hit on a loop end point
			if (B.Get(nextbindex) == B.Get(j + 1) || (AlmostEqual(B.Get(nextbindex).m_x, B.Get(j + 1).m_x) && AlmostEqual(B.Get(nextbindex).m_y, B.Get(j + 1).m_y)))
			{
				nextbindex = (nextbindex == B.Size() - 1) ? 0 : nextbindex + 1;
			}

			if (A.Get(nextaindex) == A.Get(i + 1) || (AlmostEqual(A.Get(nextaindex).m_x, A.Get(i + 1).m_x) && AlmostEqual(A.Get(nextaindex).m_y, A.Get(i + 1).m_y)))
			{
				nextaindex = (nextaindex == A.Size() - 1) ? 0 : nextaindex + 1;
			}

			Segments a0(A.Get(prevaindex).m_x + Aoffsetx, A.Get(prevaindex).m_y + Aoffsety);
			Segments b0(B.Get(prevbindex).m_x + Boffsetx, B.Get(prevbindex).m_y + Boffsety);

			Segments a3(A.Get(nextaindex).m_x + Aoffsetx, A.Get(nextaindex).m_y + Aoffsety);
			Segments b3(B.Get(nextbindex).m_x + Boffsetx, B.Get(nextbindex).m_y + Boffsety);

			if (OnSegment(a1, a2, b1) || (AlmostEqual(a1.m_x, b1.m_x, 0.01) && AlmostEqual(a1.m_y, b1.m_y, 0.01)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
				bool b0in = PointInPolygon(b0, A);
				bool b2in = PointInPolygon(b2, A);
				if ((b0in == true && b2in == false) || (b0in == false && b2in == true)) 
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

			if (OnSegment(a1, a2, b2) || (AlmostEqual(a2.m_x, b2.m_x) && AlmostEqual(a2.m_y, b2.m_y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
				bool b1in = PointInPolygon(b1, A);
				bool b3in = PointInPolygon(b3, A);
				if ((b1in == true && b3in == false) || (b1in == false && b3in == true)) 
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

			if (OnSegment(b1, b2, a1) || (AlmostEqual(a1.m_x, b2.m_x) && AlmostEqual(a1.m_y, b2.m_y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
				bool a0in = PointInPolygon(a0, B);
				bool a2in = PointInPolygon(a2, B);
				if ((a0in == true && a2in == false) || (a0in == false && a2in == true)) 
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

			if (OnSegment(b1, b2, a2) || (AlmostEqual(a2.m_x, b1.m_x) && AlmostEqual(a2.m_y, b1.m_y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
				bool a1in = PointInPolygon(a1, B);
				bool a3in = PointInPolygon(a3, B);
				if ((a1in == true && a3in == false) || (a1in == false && a3in == true)) 
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

			Segments* p = LineIntersect(b1, b2, a1, a2, false);

			if (p != NULL) 
			{

				return true;
			}
		}
	}

	return false;
}

Segments* GeometryUtil::LineIntersect(Segments A, Segments B, Segments E, Segments F, bool infinite) 
{
	double a1, a2, b1, b2, c1, c2, x, y;

	a1 = B.m_y - A.m_y;
	b1 = A.m_x - B.m_x;
	c1 = B.m_x*A.m_y - A.m_x*B.m_y;
	a2 = F.m_y - E.m_y;
	b2 = E.m_x - F.m_x;
	c2 = F.m_x*E.m_y - E.m_x*F.m_y;

	double denom = a1 * b2 - a2 * b1;

	x = (b1*c2 - b2 * c1) / denom;
	y = (a2*c1 - a1 * c2) / denom;

	if (!isinf(x) || !isinf(y))
	{
		//            System.out.println(" not infi ");
		return NULL;
	}

	if (!infinite) 
	{
		// coincident points do not count as intersecting
		if (abs(A.m_x - B.m_x) > m_tol && ((A.m_x < B.m_x) ? x < A.m_x || x > B.m_x : x > A.m_x || x < B.m_x)) return NULL;
		if (abs(A.m_y - B.m_y) > m_tol && ((A.m_y < B.m_y) ? y < A.m_y || y > B.m_y : y > A.m_y || y < B.m_y)) return NULL;

		if (abs(E.m_x - F.m_x) > m_tol && ((E.m_x < F.m_x) ? x < E.m_x || x > F.m_x : x > E.m_x || x < F.m_x)) return NULL;
		if (abs(E.m_y - F.m_y) > m_tol && ((E.m_y < F.m_y) ? y < E.m_y || y > F.m_y : y > E.m_y || y < F.m_y)) return NULL;
	}
	return new Segments(x, y);
}

double GeometryUtil::PolygonSlideDistance(NestPath TA, NestPath TB, Segments direction, bool ignoreNegative)
{
	double Aoffsetx = TA.m_offsetX;
	double Aoffsety = TA.m_offsetY;

	double Boffsetx = TB.m_offsetX;
	double BoffsetY = TB.m_offsetY;

	NestPath A(TA);
	NestPath B(TB);

	if (A.Get(0) != A.Get(A.Size() - 1))
	{
		A.Add(A.Get(0));
	}
	if (B.Get(0) != B.Get(B.Size() - 1))
	{
		B.Add(B.Get(0));
	}

	NestPath edgeA = A;
	NestPath edgeB = B;

	double distance = -1;


	Segments dir = NormalizeVector(direction);

	Segments normal(dir.m_y, -dir.m_x);

	Segments reverse(-dir.m_x, -dir.m_y);

	Segments A1, A2, B1, B2;
	for (int i = 0; i < edgeB.Size() - 1; i++) 
	{
		for (int j = 0; j < edgeA.Size() - 1; j++) 
		{
			A1 = Segments(edgeA.Get(j).m_x + Aoffsetx, edgeA.Get(j).m_y + Aoffsety);
			A2 = Segments(edgeA.Get(j + 1).m_x + Aoffsetx, edgeA.Get(j + 1).m_y + Aoffsety);
			B1 = Segments(edgeB.Get(i).m_x + Boffsetx, edgeB.Get(i).m_y + BoffsetY);
			B2 = Segments(edgeB.Get(i + 1).m_x + Boffsetx, edgeB.Get(i + 1).m_y + BoffsetY);

			if ((AlmostEqual(A1.m_x, A2.m_x) && AlmostEqual(A1.m_y, A2.m_y)) || (AlmostEqual(B1.m_x, B2.m_x) && AlmostEqual(B1.m_y, B2.m_y)))
			{
				continue;
			}
			double d = SegmentDistance(A1, A2, B1, B2, dir);

			if (d != -1 && (distance == -1 || d < distance)) 
			{
				if (!ignoreNegative || d > 0 || AlmostEqual(d, 0)) 
				{
					distance = d;
				}
			}
		}
	}
	return distance;
}

Segments GeometryUtil::NormalizeVector(Segments v) 
{
	if (AlmostEqual(v.m_x * v.m_x + v.m_y * v.m_y, 1))
	{
		return v;
	}
	double len = sqrt(v.m_x * v.m_x + v.m_y *v.m_y);
	double inverse = 1 / len;

	return Segments(v.m_x * inverse, v.m_y * inverse);
}

double GeometryUtil::SegmentDistance(Segments A, Segments B, Segments E, Segments F, Segments direction) 
{
	double SEGTOL = 10E-4;
	Segments normal(direction.m_y, -direction.m_x);

	Segments reverse(-direction.m_x, -direction.m_y);

	double dotA = A.m_x*normal.m_x + A.m_y*normal.m_y;
	double dotB = B.m_x*normal.m_x + B.m_y*normal.m_y;
	double dotE = E.m_x*normal.m_x + E.m_y*normal.m_y;
	double dotF = F.m_x*normal.m_x + F.m_y*normal.m_y;

	double crossA = A.m_x*direction.m_x + A.m_y*direction.m_y;
	double crossB = B.m_x*direction.m_x + B.m_y*direction.m_y;
	double crossE = E.m_x*direction.m_x + E.m_y*direction.m_y;
	double crossF = F.m_x*direction.m_x + F.m_y*direction.m_y;
	double crossABmin = min(crossA, crossB);
	double crossABmax = max(crossA, crossB);

	double crossEFmax = max(crossE, crossF);
	double crossEFmin = min(crossE, crossF);

	double ABmin = min(dotA, dotB);
	double ABmax = max(dotA, dotB);

	double EFmax = max(dotE, dotF);
	double EFmin = min(dotE, dotF);

	if (AlmostEqual(ABmax, EFmin, SEGTOL) || AlmostEqual(ABmin, EFmax, SEGTOL)) 
	{
		return -1;
	}
	// segments miss eachother completely
	if (ABmax < EFmin || ABmin > EFmax) 
	{
		return -1;
	}
	double overlap;
	if ((ABmax > EFmax && ABmin < EFmin) || (EFmax > ABmax && EFmin < ABmin)) 
	{
		overlap = 1;
	}
	else 
	{
		double minMax = min(ABmax, EFmax);
		double maxMin = max(ABmin, EFmin);

		double maxMax = max(ABmax, EFmax);
		double minMin = min(ABmin, EFmin);

		overlap = (minMax - maxMin) / (maxMax - minMin);
	}
	double crossABE = (E.m_y - A.m_y) * (B.m_x - A.m_x) - (E.m_x - A.m_x) * (B.m_y - A.m_y);
	double crossABF = (F.m_y - A.m_y) * (B.m_x - A.m_x) - (F.m_x - A.m_x) * (B.m_y - A.m_y);

	if (AlmostEqual(crossABE, 0) && AlmostEqual(crossABF, 0)) 
	{

		Segments ABnorm(B.m_y - A.m_y, A.m_x - B.m_x);
		Segments EFnorm(F.m_y - E.m_y, E.m_x - F.m_x);

		double ABnormlength = sqrt(ABnorm.m_x*ABnorm.m_x + ABnorm.m_y*ABnorm.m_y);
		ABnorm.m_x /= ABnormlength;
		ABnorm.m_y /= ABnormlength;

		double EFnormlength = sqrt(EFnorm.m_x*EFnorm.m_x + EFnorm.m_y*EFnorm.m_y);
		EFnorm.m_x /= EFnormlength;
		EFnorm.m_y /= EFnormlength;

		// Segments normals must point in opposite directions
		if (abs(ABnorm.m_y * EFnorm.m_x - ABnorm.m_x * EFnorm.m_y) < SEGTOL && ABnorm.m_y * EFnorm.m_y + ABnorm.m_x * EFnorm.m_x < 0)
		{
			// normal of AB Segments must point in same direction as given direction vector
			double normdot = ABnorm.m_y * direction.m_y + ABnorm.m_x * direction.m_x;
			// the segments merely slide along eachother
			if (AlmostEqual(normdot, 0, SEGTOL)) 
			{
				return -1;
			}
			if (normdot < 0) 
			{
				return (double)0;
			}
		}
		return -1;
	}
	vector<double> distances;

	// coincident points
	if (AlmostEqual(dotA, dotE)) 
	{
		distances.push_back(crossA - crossE);
	}
	else if (AlmostEqual(dotA, dotF)) 
	{
		distances.push_back(crossA - crossF);
	}
	else if (dotA > EFmin && dotA < EFmax) 
	{
		double d = PointDistance(A, E, F, reverse, false);
		if (d != -1 && AlmostEqual(d, 0)) { //  A currently touches EF, but AB is moving away from EF
			double dB = PointDistance(B, E, F, reverse, true);
			if (dB < 0 || AlmostEqual(dB*overlap, 0)) 
			{
				d = -1;
			}
		}
		if (d != -1)
		{
			distances.push_back(d);
		}
	}

	if (AlmostEqual(dotB, dotE)) 
	{
		distances.push_back(crossB - crossE);
	}
	else if (AlmostEqual(dotB, dotF)) 
	{
		distances.push_back(crossB - crossF);
	}
	else if (dotB > EFmin && dotB < EFmax) 
	{
		double d = PointDistance(B, E, F, reverse, false);

		if (d != -1 && AlmostEqual(d, 0)) 
		{ // crossA>crossB A currently touches EF, but AB is moving away from EF
			double dA = PointDistance(A, E, F, reverse, true);
			if (dA < 0 || AlmostEqual(dA*overlap, 0)) 
			{
				d = -1;
			}
		}
		if (d != -1) 
		{
			distances.push_back(d);
		}
	}

	if (dotE > ABmin && dotE < ABmax) 
	{
		double d = PointDistance(E, A, B, direction, false);
		if (d != -1 && AlmostEqual(d, 0)) 
		{ // crossF<crossE A currently touches EF, but AB is moving away from EF
			double dF = PointDistance(F, A, B, direction, true);
			if (dF < 0 || AlmostEqual(dF*overlap, 0)) 
			{
				d = -1;
			}
		}
		if (d != -1) 
		{
			distances.push_back(d);
		}
	}

	if (dotF > ABmin && dotF < ABmax) 
	{
		double d = PointDistance(F, A, B, direction, false);
		if (d != -1 && AlmostEqual(d, 0)) 
		{ // && crossE<crossF A currently touches EF, but AB is moving away from EF
			double dE = PointDistance(E, A, B, direction, true);
			if (dE < 0 || AlmostEqual(dE*overlap, 0)) 
			{
				d = -1;
			}
		}
		if (d != -1) 
		{
			distances.push_back(d);
		}
	}

	if (distances.size() == 0)
	{
		return -1;
	}

	double minElement = DBL_MAX;
	//for (double d : distances) 
	double d;
	for (int i = 0; i < distances.size(); i++)
	{
		d = distances.at(i);
		if (d < minElement)
		{
			minElement = d;
		}
	}
	return minElement;
}

double GeometryUtil::PointDistance(Segments p, Segments s1, Segments s2, Segments normal, bool infinite) 
{
	normal = NormalizeVector(normal);
	Segments dir(normal.m_y, -normal.m_x);

	double pdot = p.m_x*dir.m_x + p.m_y*dir.m_y;
	double s1dot = s1.m_x*dir.m_x + s1.m_y*dir.m_y;
	double s2dot = s2.m_x*dir.m_x + s2.m_y*dir.m_y;

	double pdotnorm = p.m_x*normal.m_x + p.m_y*normal.m_y;
	double s1dotnorm = s1.m_x*normal.m_x + s1.m_y*normal.m_y;
	double s2dotnorm = s2.m_x*normal.m_x + s2.m_y*normal.m_y;


	if (!infinite) 
	{
		if (((pdot < s1dot || AlmostEqual(pdot, s1dot)) && (pdot < s2dot || AlmostEqual(pdot, s2dot))) || ((pdot > s1dot || AlmostEqual(pdot, s1dot)) && (pdot > s2dot || AlmostEqual(pdot, s2dot)))) {
			return -1; // dot doesn't collide with Segments, or lies directly on the vertex
		}
		if ((AlmostEqual(pdot, s1dot) && AlmostEqual(pdot, s2dot)) && (pdotnorm > s1dotnorm && pdotnorm > s2dotnorm)) 
		{
			return min(pdotnorm - s1dotnorm, pdotnorm - s2dotnorm);
		}
		if ((AlmostEqual(pdot, s1dot) && AlmostEqual(pdot, s2dot)) && (pdotnorm < s1dotnorm && pdotnorm < s2dotnorm)) 
		{
			return -min(s1dotnorm - pdotnorm, s2dotnorm - pdotnorm);
		}
	}

	return -(pdotnorm - s1dotnorm + (s1dotnorm - s2dotnorm)*(s1dot - pdot) / (s1dot - s2dot));
}

/**
	* 专门为环绕矩形生成的nfp
	* @param A
	* @param B
	* @return
	*/
vector<NestPath>* GeometryUtil::NoFitPolygonRectangle(NestPath A, NestPath B)
{
	double minAx = A.Get(0).m_x;
	double minAy = A.Get(0).m_y;
	double maxAx = A.Get(0).m_x;
	double maxAy = A.Get(0).m_y;

	for (int i = 1; i < A.Size(); i++) 
	{
		if (A.Get(i).m_x < minAx)
		{
			minAx = A.Get(i).m_x;
		}
		if (A.Get(i).m_y < minAy)
		{
			minAy = A.Get(i).m_y;
		}
		if (A.Get(i).m_x > maxAx)
		{
			maxAx = A.Get(i).m_x;
		}
		if (A.Get(i).m_y > maxAy)
		{
			maxAy = A.Get(i).m_y;
		}
	}

	double minBx = B.Get(0).m_x;
	double minBy = B.Get(0).m_y;
	double maxBx = B.Get(0).m_x;
	double maxBy = B.Get(0).m_y;
	for (int i = 1; i < B.Size(); i++) 
	{
		if (B.Get(i).m_x < minBx)
		{
			minBx = B.Get(i).m_x;
		}
		if (B.Get(i).m_y < minBy)
		{
			minBy = B.Get(i).m_y;
		}
		if (B.Get(i).m_x > maxBx)
		{
			maxBx = B.Get(i).m_x;
		}
		if (B.Get(i).m_y > maxBy)
		{
			maxBy = B.Get(i).m_y;
		}
	}



	if (maxBx - minBx > maxAx - minAx) 
	{

		return NULL;
	}
	double diffBy = maxBy - minBy;
	double diffAy = maxAy - minAy;

	if (diffBy > diffAy) 
	{
		return NULL;
	}


	nfpRect = new vector<NestPath>();
	NestPath res;
	res.Add(minAx - minBx + B.Get(0).m_x, minAy - minBy + B.Get(0).m_y);
	res.Add(maxAx - maxBx + B.Get(0).m_x, minAy - minBy + B.Get(0).m_y);
	res.Add(maxAx - maxBx + B.Get(0).m_x, maxAy - maxBy + B.Get(0).m_y);
	res.Add(minAx - minBx + B.Get(0).m_x, maxAy - maxBy + B.Get(0).m_y);
	nfpRect->push_back(res);
	return nfpRect;
}

/**
	*
	* @param A
	* @param B
	* @return
	*/
vector<NestPath> GeometryUtil::MinkowskiDifference(NestPath A, NestPath B)
{
	Path Ac = Placementworker::ScaleUp2ClipperCoordinates(A);
	Path Bc = Placementworker::ScaleUp2ClipperCoordinates(B);
	for (int i = 0; i < Bc.size(); i++) 
	{
		long X = Bc.at(i).X;
		long Y = Bc.at(i).Y;
		Bc.at(i).X = -1 * X;
		Bc.at(i).Y = -1 * Y;
	}
	Paths solution;
	MinkowskiSum(Ac, Bc, solution, true);
	double largestArea = DBL_MAX;
	NestPath clipperNfp;
	for (int i = 0; i < solution.size(); i++) 
	{

		NestPath  n = Placementworker::ToNestCoordinates(solution.at(i));
		double sarea = GeometryUtil::PolygonArea(n);
		if (largestArea > sarea) 
		{
			clipperNfp = n;
			largestArea = sarea;
		}
	}

	for (int i = 0; i < clipperNfp.Size(); i++) 
	{
		clipperNfp.Get(i).m_x += B.Get(0).m_x;
		clipperNfp.Get(i).m_y += B.Get(0).m_y;
	}
	//vector<NestPath> *nfp = new vector<NestPath>();
	vector<NestPath> nfp;
	nfp.push_back(clipperNfp);
	return nfp;
}

NestPath GeometryUtil::Linearize(Segments p1, Segments p2, double rx, double ry, double angle, int laregearc, int sweep, double tol) 
{
	NestPath finished;
	finished.Add(p2);
	DataExchange arc = ConvertToCenter(p1, p2, rx, ry, angle, laregearc, sweep);
	deque<DataExchange> list;
	list.push_back(arc);
	while (list.size() > 0) 
	{
		arc = list.front();
		DataExchange fullarc = ConvertToSvg(arc.m_center, arc.m_rx, arc.m_ry, arc.m_theta, arc.m_extent, arc.m_angle);
		DataExchange subarc = ConvertToSvg(arc.m_center, arc.m_rx, arc.m_ry, arc.m_theta, 0.5*arc.m_extent, arc.m_angle);
		Segments arcmid = subarc.m_p2;
		Segments mid(0.5*(fullarc.m_p1.m_x + fullarc.m_p2.m_x), 0.5 *(fullarc.m_p1.m_y + fullarc.m_p2.m_y));
		if (WithinDistance(mid, arcmid, tol)) 
		{
			finished.Reverse(); finished.Add(Segments(fullarc.m_p2)); finished.Reverse();
			list.pop_front();
		}
		else 
		{
			DataExchange arc1(Segments(arc.m_center), arc.m_rx, arc.m_ry, arc.m_theta, 0.5 * arc.m_extent, arc.m_angle, false);
			DataExchange arc2(Segments(arc.m_center), arc.m_rx, arc.m_ry, arc.m_theta + 0.5 * arc.m_extent, 0.5 * arc.m_extent, arc.m_angle, false);
			list.pop_front();
			list.push_front(arc2); list.push_front(arc1);
		}
	}
	return finished;
}

DataExchange GeometryUtil::ConvertToSvg(Segments center, double rx, double ry, double theta1, double extent, double angleDegrees) 
{
	double theta2 = theta1 + extent;

	theta1 = DegreesToRadians(theta1);
	theta2 = DegreesToRadians(theta2);
	double angle = DegreesToRadians(angleDegrees);

	double cos1 = cos(angle);
	double sin1 = sin(angle);

	double t1cos1 = cos(theta1);
	double t1sin1 = sin(theta1);

	double t2cos1 = cos(theta2);
	double t2sin1 = sin(theta2);

	double x0 = center.m_x + cos1 * rx * t1cos1 + (-sin1) * ry * t1sin1;
	double y0 = center.m_y + sin1 * rx * t1cos1 + cos1 * ry * t1sin1;

	double x1 = center.m_x + cos1 * rx * t2cos1 + (-sin1) * ry * t2sin1;
	double y1 = center.m_y + sin1 * rx * t2cos1 + cos1 * ry * t2sin1;

	int largearc = (extent > 180) ? 1 : 0;
	int sweep = (extent > 0) ? 1 : 0;
	vector<Segments> list;
	list.push_back(Segments(x0, y0)); list.push_back(Segments(x1, y1));
	return DataExchange(Segments(x0, y0), Segments(x1, y1), rx, ry, angle, largearc, sweep, true);
}

DataExchange GeometryUtil::ConvertToCenter(Segments p1, Segments p2, double rx, double ry, double angleDgrees, int largearc, int sweep) 
{
	Segments mid(0.5 *(p1.m_x + p2.m_x), 0.5 *(p1.m_y + p2.m_y));
	Segments diff(0.5 *(p2.m_x - p1.m_x), 0.5 * (p2.m_y - p1.m_y));

	double angle = DegreesToRadians(angleDgrees);
	double cos1 = cos(angle);
	double sin1 = sin(angle);

	double x1 = cos1 * diff.m_x + sin1 * diff.m_y;
	double y1 = -sin1 * diff.m_x + cos1 * diff.m_y;

	rx = abs(rx);
	ry = abs(ry);
	double Prx = rx * rx;
	double Pry = ry * ry;
	double Px1 = x1 * x1;
	double Py1 = y1 * y1;

	double radiiCheck = Px1 / Prx + Py1 / Pry;
	double radiiSqrt = sqrt(radiiCheck);
	if (radiiCheck > 1) 
	{
		rx = radiiSqrt * rx;
		ry = radiiSqrt * ry;
		Prx = rx * rx;
		Pry = ry * ry;
	}

	double sign = (largearc != sweep) ? -1 : 1;
	double sq = ((Prx * Pry) - (Prx * Py1) - (Pry * Px1)) / ((Prx * Py1) + (Pry * Px1));

	sq = (sq < 0) ? 0 : sq;

	double coef = sign * sqrt(sq);
	double cx1 = coef * ((rx * y1) / ry);
	double cy1 = coef * -((ry * x1) / rx);

	double cx = mid.m_x + (cos1 * cx1 - sin1 * cy1);
	double cy = mid.m_y + (sin1 * cx1 + cos1 * cy1);

	double ux = (x1 - cx1) / rx;
	double uy = (y1 - cy1) / ry;
	double vx = (-x1 - cx1) / rx;
	double vy = (-y1 - cy1) / ry;
	double n = sqrt((ux * ux) + (uy * uy));
	double p = ux;
	sign = (uy < 0) ? -1 : 1;

	double theta = sign * acos(p / n);
	theta = RadiansToDegree(theta);

	n = sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
	p = ux * vx + uy * vy;
	sign = ((ux * vy - uy * vx) < 0) ? -1 : 1;
	double delta = sign * acos(p / n);
	delta = RadiansToDegree(delta);

	if (sweep == 1 && delta > 0)
	{
		delta -= 360;
	}
	else if (sweep == 0 && delta < 0)
	{
		delta += 360;
	}
	double delta_dbl = delta - (int)delta;
	double theta_dbl = theta - (int)theta;
	delta = (int)delta % 360 + delta_dbl;
	theta = (int)theta % 360 + theta_dbl;
	vector<Segments> list;
	list.push_back(Segments(cx, cy));
	return DataExchange(Segments(cx, cy), rx, ry, theta, delta, angleDgrees, false);

}

double GeometryUtil::DegreesToRadians(double angle) 
{
	return angle * (PI / 180);
}

double GeometryUtil::RadiansToDegree(double angle)
{
	return angle * (180 / PI);
}

bool GeometryUtil::WithinDistance(Segments p1, Segments p2, double distance)
{
	double dx = p1.m_x - p2.m_x;
	double dy = p1.m_y - p2.m_y;
	return ((dx * dx + dy * dy) < distance * distance);
}

