#include "qDebug"
#include "GeometryUtil.h"
#define PI 3.1415926

vector<NestPath> *nfpRect;

double GeometryUtil::tol = pow(10, -2);

bool GeometryUtil::almostEqual(double a, double b)
{
    return abs(a - b) < tol;
}

bool GeometryUtil::almostEqual(double a, double b, double tolerance)
{
	return abs(a - b) < tolerance;
}

double GeometryUtil::polygonArea(NestPath polygon)
{
	double area = 0;
	for (int i = 0, j = polygon.Size() - 1; i < polygon.Size(); j = i++) 
	{
		Segments si = polygon.GetSegments()->at(i);
		Segments sj = polygon.GetSegments()->at(j);
        area += (sj.getX() + si.getX()) * (sj.getY() - si.getY());
	}
	return 0.5*area;
}

bool GeometryUtil::onSegment(Segments A, Segments B, Segments p)
{
    if (almostEqual(A.x, B.x) && almostEqual(p.x, A.x))
	{
        if (!almostEqual(p.y, B.y) && !almostEqual(p.y, A.y) && p.y < max(B.y, A.y) && p.y > min(B.y, A.y))
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

    if (almostEqual(A.y, B.y) && almostEqual(p.y, A.y))
	{
        if (!almostEqual(p.x, B.x) && !almostEqual(p.x, A.x) && p.x < max(B.x, A.x) && p.x > min(B.x, A.x))
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

    if ((p.x < A.x && p.x < B.x) || (p.x > A.x && p.x > B.x) || (p.y < A.y && p.y < B.y) || (p.y > A.y && p.y > B.y))
	{
		return false;
	}


    if ((almostEqual(p.x, A.x) && almostEqual(p.y, A.y)) || (almostEqual(p.x, B.x) && almostEqual(p.y, B.y)))
	{
		return false;
	}

    double cross = (p.y - A.y) * (B.x - A.x) - (p.x - A.x) * (B.y - A.y);

    if (abs(cross) > tol)
	{
		return false;
	}

    double dot = (p.x - A.x) * (B.x - A.x) + (p.y - A.y)*(B.y - A.y);

    if (dot < 0 || almostEqual(dot, 0))
	{
		return false;
	}

    double len2 = (B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y);

    if (dot > len2 || almostEqual(dot, len2))
	{
		return false;
	}

	return true;
}

bool GeometryUtil::pointInPolygon(Segments point, NestPath polygon)
{
	bool inside = false;
    double offsetX = polygon.offsetX;
    double offsetY = polygon.offsetY;

	for (int i = 0, j = polygon.Size() - 1; i < polygon.Size(); j = i++) 
	{
        double xi = polygon.Get(i).x + offsetX;
        double yi = polygon.Get(i).y + offsetY;
        double xj = polygon.Get(j).x + offsetX;
        double yj = polygon.Get(j).y + offsetY;

        if (almostEqual(xi, point.x) && almostEqual(yi, point.y))
		{
			return inside; // no result
		}

        if (onSegment(Segments(xi, yi), Segments(xj, yj), point))
		{
			return inside; // exactly on the Segments
		}

        if (almostEqual(xi, xj) && almostEqual(yi, yj)) { // ignore very small lines
			continue;
		}

        bool intersect = ((yi > point.y) != (yj > point.y)) && (point.x < (xj - xi) * (point.y - yi) / (yj - yi) + xi);
		if (intersect)
		{
			inside = !inside;
		}
	}
	return inside;
}

Bound GeometryUtil::getPolygonBounds(NestPath polygon)
{
    double minX = polygon.GetSegments()->at(0).getX();
    double maxX = polygon.GetSegments()->at(0).getX();
    double minY = polygon.GetSegments()->at(0).getY();
    double maxY = polygon.GetSegments()->at(0).getY();

	for (int i = 1; i < polygon.GetSegments()->size(); i++)
	{
        double x = polygon.GetSegments()->at(i).getX();
        double y = polygon.GetSegments()->at(i).getY();
        if (x > maxX)
		{
            maxX = x;
		}
        else if (x < minX)
		{
            minX = x;
		}

        if (y > maxY)
		{
            maxY = y;
		}
        else if (y < minY)
		{
            minY = y;
		}
	}
    return Bound(minX, minY, maxX - minX, maxY - minY);
}

Bound GeometryUtil::rotatePolygon(NestPath polygon, int angle)
{
	if (angle == 0) 
	{
        return getPolygonBounds(polygon);
	}
    double fAngle = angle * PI / 180;
	NestPath rotated;
	for (int i = 0; i < polygon.Size(); i++) 
	{
        double x = polygon.Get(i).x;
        double y = polygon.Get(i).y;
        double x1 = x * cos(fAngle) - y * sin(fAngle);
        double y1 = x * sin(fAngle) + y * cos(fAngle);
		rotated.Add(x1, y1);
	}
    Bound bounds = getPolygonBounds(rotated);
	return bounds;
}

NestPath GeometryUtil::rotatePolygon2Polygon(NestPath polygon, int degrees)
{
	NestPath rotated;
	double angle = degrees * PI / 180;
	for (int i = 0; i < polygon.Size(); i++) 
	{
        double x = polygon.Get(i).x;
        double y = polygon.Get(i).y;
        double translateX = x * cos(angle) - y * sin(angle);
        double translateY = x * sin(angle) + y * cos(angle);
        rotated.Add(Segments(translateX, translateY));
	}
    rotated.bid = polygon.bid;
    rotated.setId(polygon.getId());
    rotated.setSource(polygon.getSource());
    if (polygon.getChildren()->size() > 0)
	{
        for (int j = 0; j < polygon.getChildren()->size(); j++)
		{
            rotated.getChildren()->push_back(rotatePolygon2Polygon(polygon.getChildren()->at(j), degrees));
		}
	}
	return rotated;
}

bool GeometryUtil::isRectangle(NestPath poly, double tolerance)
{
    Bound bb = getPolygonBounds(poly);

	for (int i = 0; i < poly.Size(); i++) 
	{
        if (!almostEqual(poly.Get(i).x, bb.getMinX(), tolerance) && !almostEqual(poly.Get(i).x, bb.getMinX() + bb.getWidth(), tolerance))
		{
			return false;
		}
        if (!almostEqual(poly.Get(i).y, bb.getMinY(), tolerance) && !almostEqual(poly.Get(i).y, bb.getMinY() + bb.getHeight(), tolerance))
		{
			return false;
		}
	}
	return true;
}

vector<NestPath> GeometryUtil::noFitPolygon(NestPath A, NestPath B, bool inside, bool searchEdges)
{
    A.setOffsetX(0);
    A.setOffsetY(0);

    double minA = A.Get(0).y;
	int minAIndex = 0;
    double currentAX = A.Get(0).x;
    double maxB = B.Get(0).y;
	int maxBIndex = 0;

	for (int i = 1; i < A.Size(); i++) 
	{
        A.Get(i).marked = false;
		
        if (almostEqual(A.Get(i).y, minA) && A.Get(i).x < currentAX)
		{
            minA = A.Get(i).y;
			minAIndex = i;
            currentAX = A.Get(i).x;
		}
        else if (A.Get(i).y < minA)
		{
            minA = A.Get(i).y;
			minAIndex = i;
            currentAX = A.Get(i).x;
		}
	}
	for (int i = 1; i < B.Size(); i++) 
	{
        B.Get(i).marked = false;
        if (B.Get(i).y > maxB)
		{
            maxB = B.Get(i).y;
			maxBIndex = i;
		}
	}
	Segments *startPoint = NULL;
	if (!inside) 
	{
        startPoint = new Segments(A.Get(minAIndex).x - B.Get(maxBIndex).x,
            A.Get(minAIndex).y - B.Get(maxBIndex).y);

	}
	else 
	{
        vector<NestPath> nestPathList;
        startPoint = searchStartPoint(A, B, true, nestPathList);

	}

    vector<NestPath> *nfpList = new vector<NestPath>();

	while (startPoint != NULL)
	{
        Segments *prevVector = NULL;
        B.setOffsetX(startPoint->x);
        B.setOffsetY(startPoint->y);


		vector<SegmentRelation> *touching;
		NestPath* NFP = new NestPath();
        NFP->Add(Segments(B.Get(0).x + B.getOffsetX(),
            B.Get(0).y + B.getOffsetY()));

        double referenceX = B.Get(0).x + B.getOffsetX();
        double referenceY = B.Get(0).y + B.getOffsetY();
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
                    if (almostEqual(A.Get(i).x, B.Get(j).x + B.offsetX) && almostEqual(A.Get(i).y, B.Get(j).y + B.offsetY))
					{
						touching->push_back(SegmentRelation(0, i, j));
					}
                    else if (onSegment(A.Get(i), A.Get(nexti), Segments(B.Get(j).x + B.offsetX, B.Get(j).y + B.offsetY)))
					{
						touching->push_back(SegmentRelation(1, nexti, j));
					}
                    else if (onSegment(Segments(B.Get(j).x + B.offsetX, B.Get(j).y + B.offsetY),
                        Segments(B.Get(nextj).x + B.offsetX, B.Get(nextj).y + B.offsetY),
						A.Get(i))) 
					{
						touching->push_back(SegmentRelation(2, i, nextj));
					}
				}
			}


			NestPath *vectors = new NestPath();
			for (int i = 0; i < touching->size(); i++)
			{
                Segments vertexA = A.Get(touching->at(i).a);
                vertexA.marked = true;

                int prevAIndex = touching->at(i).a - 1;
                int nextAIndex = touching->at(i).a + 1;

				prevAIndex = (prevAIndex < 0) ? A.Size() - 1 : prevAIndex; // loop
				nextAIndex = (nextAIndex >= A.Size()) ? 0 : nextAIndex; // loop

				Segments prevA = A.Get(prevAIndex);
				Segments nextA = A.Get(nextAIndex);

                Segments vertexB = B.Get(touching->at(i).b);

                int prevBIndex = touching->at(i).b - 1;
                int nextBIndex = touching->at(i).b + 1;

				prevBIndex = (prevBIndex < 0) ? B.Size() - 1 : prevBIndex; // loop
				nextBIndex = (nextBIndex >= B.Size()) ? 0 : nextBIndex; // loop

				Segments prevB = B.Get(prevBIndex);
				Segments nextB = B.Get(nextBIndex);

                if (touching->at(i).type == 0) {
                    Segments vA1(prevA.x - vertexA.x, prevA.y - vertexA.y);
                    vA1.start = &vertexA; vA1.end = &prevA;

                    Segments vA2(nextA.x - vertexA.x, nextA.y - vertexA.y);
                    vA2.start = &vertexA; vA2.end = &nextA;

                    Segments vB1(vertexB.x - prevB.x, vertexB.y - prevB.y);
                    vB1.start = &prevB; vB1.end = &vertexB;

                    Segments vB2(vertexB.x - nextB.x, vertexB.y - nextB.y);
                    vB2.start = &nextB; vB2.end = &vertexB;

					vectors->Add(vA1);
					vectors->Add(vA2);
					vectors->Add(vB1);
					vectors->Add(vB2);
				}
                else if (touching->at(i).type == 1)
				{

                    Segments tmp(vertexA.x - (vertexB.x + B.offsetX),
                        vertexA.y - (vertexB.y + B.offsetY));

                    tmp.start = &prevA;
                    tmp.end = &vertexA;

                    Segments tmp2(prevA.x - (vertexB.x + B.offsetX), prevA.y - (vertexB.y + B.offsetY));
                    tmp2.start = &vertexA; tmp2.end = &prevA;
					vectors->Add(tmp);
					vectors->Add(tmp2);

				}
                else if (touching->at(i).type == 2)
				{
                    Segments tmp1(vertexA.x - (vertexB.x + B.offsetX),
                        vertexA.y - (vertexB.y + B.offsetY));
                    tmp1.start = &prevB;
                    tmp1.end = &vertexB;
                    Segments tmp2(vertexA.x - (prevB.x + B.offsetX),
                        vertexA.y - (prevB.y + B.offsetY));
                    tmp2.start = &vertexB;
                    tmp2.end = &prevB;

					vectors->Add(tmp1); vectors->Add(tmp2);
				}
			}

			Segments *translate = NULL;

			double maxd = 0.0;
			for (int i = 0; i < vectors->Size(); i++)
			{
                if (almostEqual(vectors->Get(i).x, 0) && almostEqual(vectors->Get(i).y, 0))
				{
					continue;
				}

                if (prevVector != NULL && vectors->Get(i).y * prevVector->y + vectors->Get(i).x * prevVector->x < 0)
				{

                    double vectorLength = sqrt(vectors->Get(i).x*vectors->Get(i).x + vectors->Get(i).y*vectors->Get(i).y);
                    Segments unitv(vectors->Get(i).x / vectorLength, vectors->Get(i).y / vectorLength);


                    double prevLength = sqrt(prevVector->x*prevVector->x + prevVector->y*prevVector->y);
                    Segments prevUnit(prevVector->x / prevLength, prevVector->y / prevLength);


					// we need to scale down to unit vectors to normalize vector length. Could also just do a tan here
                    if (abs(unitv.y * prevUnit.x - unitv.x * prevUnit.y) < 0.0001)
					{

						continue;
					}
				}
				//todo polygonSlideDistance
                double d = polygonSlideDistance(A, B, vectors->Get(i), true);

                double vecd2 = vectors->Get(i).x*vectors->Get(i).x + vectors->Get(i).y*vectors->Get(i).y;

				if (d == -1 || d * d > vecd2) 
				{
                    double vecd = sqrt(vectors->Get(i).x*vectors->Get(i).x + vectors->Get(i).y*vectors->Get(i).y);
					d = vecd;
				}

				if (d != -1 && d > maxd) 
				{
					maxd = d;
					translate = &(vectors->Get(i));
				}

			}

            if (translate == NULL || almostEqual(maxd, 0))
			{
				// didn't close the loop, something went wrong here
				if (translate == NULL)
				{

				}
                if (almostEqual(maxd, 0))
				{
				}
				NFP = NULL;
				break;
			}

            translate->start->marked = true;
            translate->end->marked = true;

            prevVector = translate;


			// trim
            double vLength2 = translate->x*translate->x + translate->y*translate->y;
            if (maxd*maxd < vLength2 && !almostEqual(maxd*maxd, vLength2))
			{
                double scale = sqrt((maxd*maxd) / vLength2);
                translate->x *= scale;
                translate->y *= scale;
			}

            referenceX += translate->x;
            referenceY += translate->y;


            if (almostEqual(referenceX, startX) && almostEqual(referenceY, startY))
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
                    if (almostEqual(referenceX, NFP->Get(i).x) && almostEqual(referenceY, NFP->Get(i).y))
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

            B.offsetX += translate->x;
            B.offsetY += translate->y;
			counter++;
		}

		if (NFP != NULL && NFP->Size() > 0) 
		{
            nfpList->push_back(*NFP);
		}

		if (!searchEdges) 
		{
			// only get outer NFP or first inner NFP
			break;
		}
        startPoint = searchStartPoint(A, B, inside, *nfpList);
	}
    return *nfpList;
}

Segments* GeometryUtil::searchStartPoint(NestPath CA, NestPath CB, bool inside, vector<NestPath> NFP)
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
        if (!A.Get(i).marked)
		{
            A.Get(i).marked = true;
			for (int j = 0; j < B.Size(); j++)
			{
                B.offsetX = A.Get(i).x - B.Get(j).x;
                B.offsetY = A.Get(i).y - B.Get(j).y;
                bool bInside = false;
				for (int k = 0; k < B.Size(); k++)
				{
                    bool inpoly = pointInPolygon(Segments(B.Get(k).x + B.offsetX, B.Get(k).y + B.offsetY), A);
					//if (inpoly != null) 
					{
                        bInside = inpoly;
						break;
					}
				}

                if (bInside == false)
				{ // A and B are the same
					return NULL;
				}

                Segments* startPoint = new Segments(B.offsetX, B.offsetY);

                if (((bInside != false && inside) || (bInside == false && !inside)) && !intersect(A, B) && !inNfp(*startPoint, NFP))
				{
					return startPoint;
				}

				// slide B along vector
                double vx = A.Get(i + 1).x - A.Get(i).x;
                double vy = A.Get(i + 1).y - A.Get(i).y;

                double d1 = polygonProjectionDistance(A, B, Segments(vx, vy));
                double d2 = polygonProjectionDistance(B, A, Segments(-vx, -vy));

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
                if (d != -1 && !almostEqual(d, 0) && d > 0)
				{

				}
				else 
				{
					continue;
				}

				double vd2 = vx * vx + vy * vy;

                if (d*d < vd2 && !almostEqual(d*d, vd2))
				{
					double vd = sqrt(vx*vx + vy * vy);
					vx *= d / vd;
					vy *= d / vd;
				}

                B.offsetX += vx;
                B.offsetY += vy;

				for (int k = 0; k < B.Size(); k++)
				{
                    bool inpoly = pointInPolygon(Segments(B.Get(k).x + B.offsetX, B.Get(k).y + B.offsetY), A);
					//if (inpoly != null) 
					{
                        bInside = inpoly;
						break;
					}
				}
                startPoint = new Segments(B.offsetX, B.offsetY);
                if (((bInside && inside) || (!bInside && !inside)) && !intersect(A, B) && !inNfp(*startPoint, NFP))
				{
					return startPoint;
				}
			}
		}
	}
	return NULL;
}

bool GeometryUtil::inNfp(Segments p, vector<NestPath>& nfp)
{
	if (nfp.empty()) 
	{
		return false;
	}
	for (int i = 0; i < nfp.size(); i++) 
	{
		for (int j = 0; j < nfp.at(i).Size(); j++)
		{
            if (almostEqual(p.x, nfp.at(i).Get(j).x) && almostEqual(p.y, nfp.at(i).Get(j).y))
			{
				return true;
			}
		}
	}
	return false;
}

double GeometryUtil::polygonProjectionDistance(NestPath CA, NestPath CB, Segments direction)
{
    double bOffsetx = CB.offsetX;
    double bOffsety = CB.offsetY;

    double aOffsetx = CA.offsetX;
    double aOffsety = CA.offsetY;

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
        double minProjection = -1;
        Segments minP;
		for (int j = 0; j < edgeA.Size() - 1; j++) 
		{
            p = Segments(edgeB.Get(i).x + bOffsetx, edgeB.Get(i).y + bOffsety);
            s1 = Segments(edgeA.Get(j).x + aOffsetx, edgeA.Get(j).y + aOffsety);
            s2 = Segments(edgeA.Get(j + 1).x + aOffsetx, edgeA.Get(j + 1).y + aOffsety);
            if (abs((s2.y - s1.y) * direction.x - (s2.x - s1.x) * direction.y) < tol)
			{
				continue;
			}

			// project point, ignore edge boundaries
            d = pointDistance(p, s1, s2, direction, false);

            if (d != -1 && (minProjection == -1 || d < minProjection))
			{
                minProjection = d;
                minP = p;
			}
		}
        if (minProjection != -1 && (distance == -1 || minProjection > distance))
		{
            distance = minProjection;
		}
	}
	return distance;
}

bool GeometryUtil::intersect(NestPath CA, NestPath CB)
{
    double aOffsetx = CA.offsetX;
    double aOffsety = CA.offsetY;

    double bOffsetx = CB.offsetX;
    double bOffsety = CB.offsetY;

	NestPath A(CA);
	NestPath B(CB);

	for (int i = 0; i < A.Size() - 1; i++)
	{
		for (int j = 0; j < B.Size() - 1; j++)
		{
            Segments a1(A.Get(i).x + aOffsetx, A.Get(i).y + aOffsety);
            Segments a2(A.Get(i + 1).x + aOffsetx, A.Get(i + 1).y + aOffsety);
            Segments b1(B.Get(j).x + bOffsetx, B.Get(j).y + bOffsety);
            Segments b2(B.Get(j + 1).x + bOffsetx, B.Get(j + 1).y + bOffsety);


            int prevBIndex = (j == 0) ? B.Size() - 1 : j - 1;
            int prevAIndex = (i == 0) ? A.Size() - 1 : i - 1;
            int nextBIndex = (j + 1 == B.Size() - 1) ? 0 : j + 2;
            int nextAIndex = (i + 1 == A.Size() - 1) ? 0 : i + 2;

			// go even further back if we happen to hit on a loop end point
            if (B.Get(prevBIndex) == B.Get(j) || (almostEqual(B.Get(prevBIndex).x, B.Get(j).x) && almostEqual(B.Get(prevBIndex).y, B.Get(j).y)))
			{
                prevBIndex = (prevBIndex == 0) ? B.Size() - 1 : prevBIndex - 1;
			}

            if (A.Get(prevAIndex) == A.Get(i) || (almostEqual(A.Get(prevAIndex).x, A.Get(i).x) && almostEqual(A.Get(prevAIndex).y, A.Get(i).y)))
			{
                prevAIndex = (prevAIndex == 0) ? A.Size() - 1 : prevAIndex - 1;
			}

			// go even further forward if we happen to hit on a loop end point
            if (B.Get(nextBIndex) == B.Get(j + 1) || (almostEqual(B.Get(nextBIndex).x, B.Get(j + 1).x) && almostEqual(B.Get(nextBIndex).y, B.Get(j + 1).y)))
			{
                nextBIndex = (nextBIndex == B.Size() - 1) ? 0 : nextBIndex + 1;
			}

            if (A.Get(nextAIndex) == A.Get(i + 1) || (almostEqual(A.Get(nextAIndex).x, A.Get(i + 1).x) && almostEqual(A.Get(nextAIndex).y, A.Get(i + 1).y)))
			{
                nextAIndex = (nextAIndex == A.Size() - 1) ? 0 : nextAIndex + 1;
			}

            Segments a0(A.Get(prevAIndex).x + aOffsetx, A.Get(prevAIndex).y + aOffsety);
            Segments b0(B.Get(prevBIndex).x + bOffsetx, B.Get(prevBIndex).y + bOffsety);

            Segments a3(A.Get(nextAIndex).x + aOffsetx, A.Get(nextAIndex).y + aOffsety);
            Segments b3(B.Get(nextBIndex).x + bOffsetx, B.Get(nextBIndex).y + bOffsety);

            if (onSegment(a1, a2, b1) || (almostEqual(a1.x, b1.x, 0.01) && almostEqual(a1.y, b1.y, 0.01)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
                bool b0In = pointInPolygon(b0, A);
                bool b2In = pointInPolygon(b2, A);
                if ((b0In == true && b2In == false) || (b0In == false && b2In == true))
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

            if (onSegment(a1, a2, b2) || (almostEqual(a2.x, b2.x) && almostEqual(a2.y, b2.y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
                bool b1In = pointInPolygon(b1, A);
                bool b3In = pointInPolygon(b3, A);
                if ((b1In == true && b3In == false) || (b1In == false && b3In == true))
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

            if (onSegment(b1, b2, a1) || (almostEqual(a1.x, b2.x) && almostEqual(a1.y, b2.y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
                bool a0In = pointInPolygon(a0, B);
                bool a2In = pointInPolygon(a2, B);
                if ((a0In == true && a2In == false) || (a0In == false && a2In == true))
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

            if (onSegment(b1, b2, a2) || (almostEqual(a2.x, b1.x) && almostEqual(a2.y, b1.y)))
			{
				// if a point is on a Segments, it could intersect or it could not. Check via the neighboring points
                bool a1In = pointInPolygon(a1, B);
                bool a3In = pointInPolygon(a3, B);
                if ((a1In == true && a3In == false) || (a1In == false && a3In == true))
				{

					return true;
				}
				else 
				{
					continue;
				}
			}

            Segments* p = lineIntersect(b1, b2, a1, a2, false);

			if (p != NULL) 
			{

				return true;
			}
		}
	}

	return false;
}

Segments* GeometryUtil::lineIntersect(Segments A, Segments B, Segments E, Segments F, bool infinite)
{
	double a1, a2, b1, b2, c1, c2, x, y;

    a1 = B.y - A.y;
    b1 = A.x - B.x;
    c1 = B.x*A.y - A.x*B.y;
    a2 = F.y - E.y;
    b2 = E.x - F.x;
    c2 = F.x*E.y - E.x*F.y;

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
        if (abs(A.x - B.x) > tol && ((A.x < B.x) ? x < A.x || x > B.x : x > A.x || x < B.x)) return NULL;
        if (abs(A.y - B.y) > tol && ((A.y < B.y) ? y < A.y || y > B.y : y > A.y || y < B.y)) return NULL;

        if (abs(E.x - F.x) > tol && ((E.x < F.x) ? x < E.x || x > F.x : x > E.x || x < F.x)) return NULL;
        if (abs(E.y - F.y) > tol && ((E.y < F.y) ? y < E.y || y > F.y : y > E.y || y < F.y)) return NULL;
	}
	return new Segments(x, y);
}

double GeometryUtil::polygonSlideDistance(NestPath TA, NestPath TB, Segments direction, bool ignoreNegative)
{
    double aOffsetX = TA.offsetX;
    double aOffsetY = TA.offsetY;

    double bOffsetX = TB.offsetX;
    double bOffsetY = TB.offsetY;

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


    Segments dir = normalizeVector(direction);

    Segments normal(dir.y, -dir.x);

    Segments reverse(-dir.x, -dir.y);

	Segments A1, A2, B1, B2;
	for (int i = 0; i < edgeB.Size() - 1; i++) 
	{
		for (int j = 0; j < edgeA.Size() - 1; j++) 
		{
            A1 = Segments(edgeA.Get(j).x + aOffsetX, edgeA.Get(j).y + aOffsetY);
            A2 = Segments(edgeA.Get(j + 1).x + aOffsetX, edgeA.Get(j + 1).y + aOffsetY);
            B1 = Segments(edgeB.Get(i).x + bOffsetX, edgeB.Get(i).y + bOffsetY);
            B2 = Segments(edgeB.Get(i + 1).x + bOffsetX, edgeB.Get(i + 1).y + bOffsetY);

            if ((almostEqual(A1.x, A2.x) && almostEqual(A1.y, A2.y)) || (almostEqual(B1.x, B2.x) && almostEqual(B1.y, B2.y)))
			{
				continue;
			}
            double d = segmentDistance(A1, A2, B1, B2, dir);

			if (d != -1 && (distance == -1 || d < distance)) 
			{
                if (!ignoreNegative || d > 0 || almostEqual(d, 0))
				{
					distance = d;
				}
			}
		}
	}
	return distance;
}

Segments GeometryUtil::normalizeVector(Segments v)
{
    if (almostEqual(v.x * v.x + v.y * v.y, 1))
	{
		return v;
	}
    double len = sqrt(v.x * v.x + v.y *v.y);
	double inverse = 1 / len;

    return Segments(v.x * inverse, v.y * inverse);
}

double GeometryUtil::segmentDistance(Segments A, Segments B, Segments E, Segments F, Segments direction)
{
    double tolerance = 10E-4;
    Segments normal(direction.y, -direction.x);

    Segments reverse(-direction.x, -direction.y);

    double dotA = A.x*normal.x + A.y*normal.y;
    double dotB = B.x*normal.x + B.y*normal.y;
    double dotE = E.x*normal.x + E.y*normal.y;
    double dotF = F.x*normal.x + F.y*normal.y;

    double crossA = A.x*direction.x + A.y*direction.y;
    double crossB = B.x*direction.x + B.y*direction.y;
    double crossE = E.x*direction.x + E.y*direction.y;
    double crossF = F.x*direction.x + F.y*direction.y;
    double crossABMin = min(crossA, crossB);
    double crossABMax = max(crossA, crossB);

    double crossEFMax = max(crossE, crossF);
    double crossEFMin = min(crossE, crossF);

    double ABMin = min(dotA, dotB);
    double ABMax = max(dotA, dotB);

    double EFMax = max(dotE, dotF);
    double EFMin = min(dotE, dotF);

    if (almostEqual(ABMax, EFMin, tolerance) || almostEqual(ABMin, EFMax, tolerance))
	{
		return -1;
	}
	// segments miss eachother completely
    if (ABMax < EFMin || ABMin > EFMax)
	{
		return -1;
	}
	double overlap;
    if ((ABMax > EFMax && ABMin < EFMin) || (EFMax > ABMax && EFMin < ABMin))
	{
		overlap = 1;
	}
	else 
	{
        double minMax = min(ABMax, EFMax);
        double maxMin = max(ABMin, EFMin);

        double maxMax = max(ABMax, EFMax);
        double minMin = min(ABMin, EFMin);

		overlap = (minMax - maxMin) / (maxMax - minMin);
	}
    double crossABE = (E.y - A.y) * (B.x - A.x) - (E.x - A.x) * (B.y - A.y);
    double crossABF = (F.y - A.y) * (B.x - A.x) - (F.x - A.x) * (B.y - A.y);

    if (almostEqual(crossABE, 0) && almostEqual(crossABF, 0))
	{

        Segments ABNorm(B.y - A.y, A.x - B.x);
        Segments EFNorm(F.y - E.y, E.x - F.x);

        double ABNormLength = sqrt(ABNorm.x*ABNorm.x + ABNorm.y*ABNorm.y);
        ABNorm.x /= ABNormLength;
        ABNorm.y /= ABNormLength;

        double EFNormLength = sqrt(EFNorm.x*EFNorm.x + EFNorm.y*EFNorm.y);
        EFNorm.x /= EFNormLength;
        EFNorm.y /= EFNormLength;

		// Segments normals must point in opposite directions
        if (abs(ABNorm.y * EFNorm.x - ABNorm.x * EFNorm.y) < tolerance && ABNorm.y * EFNorm.y + ABNorm.x * EFNorm.x < 0)
		{
			// normal of AB Segments must point in same direction as given direction vector
            double normDot = ABNorm.y * direction.y + ABNorm.x * direction.x;
			// the segments merely slide along eachother
            if (almostEqual(normDot, 0, tolerance))
			{
				return -1;
			}
            if (normDot < 0)
			{
				return (double)0;
			}
		}
		return -1;
	}
	vector<double> distances;

	// coincident points
    if (almostEqual(dotA, dotE))
	{
		distances.push_back(crossA - crossE);
	}
    else if (almostEqual(dotA, dotF))
	{
		distances.push_back(crossA - crossF);
	}
    else if (dotA > EFMin && dotA < EFMax)
	{
        double d = pointDistance(A, E, F, reverse, false);
        if (d != -1 && almostEqual(d, 0)) { //  A currently touches EF, but AB is moving away from EF
            double dB = pointDistance(B, E, F, reverse, true);
            if (dB < 0 || almostEqual(dB*overlap, 0))
			{
				d = -1;
			}
		}
		if (d != -1)
		{
			distances.push_back(d);
		}
	}

    if (almostEqual(dotB, dotE))
	{
		distances.push_back(crossB - crossE);
	}
    else if (almostEqual(dotB, dotF))
	{
		distances.push_back(crossB - crossF);
	}
    else if (dotB > EFMin && dotB < EFMax)
	{
        double d = pointDistance(B, E, F, reverse, false);

        if (d != -1 && almostEqual(d, 0))
		{ // crossA>crossB A currently touches EF, but AB is moving away from EF
            double dA = pointDistance(A, E, F, reverse, true);
            if (dA < 0 || almostEqual(dA*overlap, 0))
			{
				d = -1;
			}
		}
		if (d != -1) 
		{
			distances.push_back(d);
		}
	}

    if (dotE > ABMin && dotE < ABMax)
	{
        double d = pointDistance(E, A, B, direction, false);
        if (d != -1 && almostEqual(d, 0))
		{ // crossF<crossE A currently touches EF, but AB is moving away from EF
            double dF = pointDistance(F, A, B, direction, true);
            if (dF < 0 || almostEqual(dF*overlap, 0))
			{
				d = -1;
			}
		}
		if (d != -1) 
		{
			distances.push_back(d);
		}
	}

    if (dotF > ABMin && dotF < ABMax)
	{
        double d = pointDistance(F, A, B, direction, false);
        if (d != -1 && almostEqual(d, 0))
		{ // && crossE<crossF A currently touches EF, but AB is moving away from EF
            double dE = pointDistance(E, A, B, direction, true);
            if (dE < 0 || almostEqual(dE*overlap, 0))
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

double GeometryUtil::pointDistance(Segments p, Segments s1, Segments s2, Segments normal, bool infinite)
{
    normal = normalizeVector(normal);
    Segments dir(normal.y, -normal.x);

    double pDot = p.x*dir.x + p.y*dir.y;
    double s1Dot = s1.x*dir.x + s1.y*dir.y;
    double s2Dot = s2.x*dir.x + s2.y*dir.y;

    double pDotNorm = p.x*normal.x + p.y*normal.y;
    double s1DotNorm = s1.x*normal.x + s1.y*normal.y;
    double s2DotNorm = s2.x*normal.x + s2.y*normal.y;


	if (!infinite) 
	{
        if (((pDot < s1Dot || almostEqual(pDot, s1Dot)) && (pDot < s2Dot || almostEqual(pDot, s2Dot))) || ((pDot > s1Dot || almostEqual(pDot, s1Dot)) && (pDot > s2Dot || almostEqual(pDot, s2Dot)))) {
			return -1; // dot doesn't collide with Segments, or lies directly on the vertex
		}
        if ((almostEqual(pDot, s1Dot) && almostEqual(pDot, s2Dot)) && (pDotNorm > s1DotNorm && pDotNorm > s2DotNorm))
		{
            return min(pDotNorm - s1DotNorm, pDotNorm - s2DotNorm);
		}
        if ((almostEqual(pDot, s1Dot) && almostEqual(pDot, s2Dot)) && (pDotNorm < s1DotNorm && pDotNorm < s2DotNorm))
		{
            return -min(s1DotNorm - pDotNorm, s2DotNorm - pDotNorm);
		}
	}

    return -(pDotNorm - s1DotNorm + (s1DotNorm - s2DotNorm)*(s1Dot - pDot) / (s1Dot - s2Dot));
}

vector<NestPath>* GeometryUtil::noFitPolygonRectangle(NestPath A, NestPath B)
{
    double minAX = A.Get(0).x;
    double minAY = A.Get(0).y;
    double maxAX = A.Get(0).x;
    double maxAY = A.Get(0).y;

	for (int i = 1; i < A.Size(); i++) 
	{
        if (A.Get(i).x < minAX)
		{
            minAX = A.Get(i).x;
		}
        if (A.Get(i).y < minAY)
		{
            minAY = A.Get(i).y;
		}
        if (A.Get(i).x > maxAX)
		{
            maxAX = A.Get(i).x;
		}
        if (A.Get(i).y > maxAY)
		{
            maxAY = A.Get(i).y;
		}
	}

    double minBX = B.Get(0).x;
    double minBY = B.Get(0).y;
    double maxBX = B.Get(0).x;
    double maxBY = B.Get(0).y;
	for (int i = 1; i < B.Size(); i++) 
	{
        if (B.Get(i).x < minBX)
		{
            minBX = B.Get(i).x;
		}
        if (B.Get(i).y < minBY)
		{
            minBY = B.Get(i).y;
		}
        if (B.Get(i).x > maxBX)
		{
            maxBX = B.Get(i).x;
		}
        if (B.Get(i).y > maxBY)
		{
            maxBY = B.Get(i).y;
		}
	}


    qDebug()<<"maxBX - minBX = "<< maxBX<<","<<minBX<<","<<maxBX - minBX;
    qDebug()<<"maxAX - minAX = "<< maxAX<<","<<minAX<<","<<maxAX - minAX;
    if (maxBX - minBX > maxAX - minAX)
	{
		return NULL;
	}
    double diffBY = maxBY - minBY;
    double diffAY = maxAY - minAY;

    qDebug()<<"diffBY = "<< maxBY<<","<<minBY<<","<<maxBY - minBY;
    qDebug()<<"diffAY = "<< maxAY<<","<<minAY<<","<<maxAY - minAY;
    if (diffBY > diffAY)
	{
		return NULL;
	}


	nfpRect = new vector<NestPath>();
	NestPath res;
    res.Add(minAX - minBX + B.Get(0).x, minAY - minBY + B.Get(0).y);
    res.Add(maxAX - maxBX + B.Get(0).x, minAY - minBY + B.Get(0).y);
    res.Add(maxAX - maxBX + B.Get(0).x, maxAY - maxBY + B.Get(0).y);
    res.Add(minAX - minBX + B.Get(0).x, maxAY - maxBY + B.Get(0).y);
	nfpRect->push_back(res);
	return nfpRect;
}

vector<NestPath> GeometryUtil::minkowskiDifference(NestPath A, NestPath B)
{
    Path Ac = Placementworker::scaleUp2ClipperCoordinates(A);
    Path Bc = Placementworker::scaleUp2ClipperCoordinates(B);
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

        NestPath  n = Placementworker::toNestCoordinates(solution.at(i));
        double sarea = GeometryUtil::polygonArea(n);
		if (largestArea > sarea) 
		{
			clipperNfp = n;
			largestArea = sarea;
		}
	}

	for (int i = 0; i < clipperNfp.Size(); i++) 
	{
        clipperNfp.Get(i).x += B.Get(0).x;
        clipperNfp.Get(i).y += B.Get(0).y;
	}
	//vector<NestPath> *nfp = new vector<NestPath>();
	vector<NestPath> nfp;
	nfp.push_back(clipperNfp);
	return nfp;
}

NestPath GeometryUtil::linearize(Segments p1, Segments p2, double rx, double ry, double angle, int laregearc, int sweep, double tol)
{
	NestPath finished;
	finished.Add(p2);
    DataExchange arc = convertToCenter(p1, p2, rx, ry, angle, laregearc, sweep);
	deque<DataExchange> list;
	list.push_back(arc);
	while (list.size() > 0) 
	{
		arc = list.front();
        DataExchange fullArc = convertToSvg(arc.center, arc.rx, arc.ry, arc.theta, arc.extent, arc.angle);
        DataExchange subArc = convertToSvg(arc.center, arc.rx, arc.ry, arc.theta, 0.5*arc.extent, arc.angle);
        Segments arcMid = subArc.p2;
        Segments mid(0.5*(fullArc.p1.x + fullArc.p2.x), 0.5 *(fullArc.p1.y + fullArc.p2.y));
        if (withinDistance(mid, arcMid, tol))
		{
            finished.reverse(); finished.Add(Segments(fullArc.p2)); finished.reverse();
			list.pop_front();
		}
		else 
		{
            DataExchange arc1(Segments(arc.center), arc.rx, arc.ry, arc.theta, 0.5 * arc.extent, arc.angle, false);
            DataExchange arc2(Segments(arc.center), arc.rx, arc.ry, arc.theta + 0.5 * arc.extent, 0.5 * arc.extent, arc.angle, false);
			list.pop_front();
			list.push_front(arc2); list.push_front(arc1);
		}
	}
	return finished;
}

DataExchange GeometryUtil::convertToSvg(Segments center, double rx, double ry, double theta1, double extent, double angleDegrees)
{
	double theta2 = theta1 + extent;

    theta1 = degreesToRadians(theta1);
    theta2 = degreesToRadians(theta2);
    double angle = degreesToRadians(angleDegrees);

	double cos1 = cos(angle);
	double sin1 = sin(angle);

    double t1Cos1 = cos(theta1);
    double t1Sin1 = sin(theta1);

    double t2Cos1 = cos(theta2);
    double t2Sin1 = sin(theta2);

    double x0 = center.x + cos1 * rx * t1Cos1 + (-sin1) * ry * t1Sin1;
    double y0 = center.y + sin1 * rx * t1Cos1 + cos1 * ry * t1Sin1;

    double x1 = center.x + cos1 * rx * t2Cos1 + (-sin1) * ry * t2Sin1;
    double y1 = center.y + sin1 * rx * t2Cos1 + cos1 * ry * t2Sin1;

    int largeArc = (extent > 180) ? 1 : 0;
	int sweep = (extent > 0) ? 1 : 0;
	vector<Segments> list;
	list.push_back(Segments(x0, y0)); list.push_back(Segments(x1, y1));
    return DataExchange(Segments(x0, y0), Segments(x1, y1), rx, ry, angle, largeArc, sweep, true);
}

DataExchange GeometryUtil::convertToCenter(Segments p1, Segments p2, double rx, double ry, double angleDgrees, int largearc, int sweep)
{
    Segments mid(0.5 *(p1.x + p2.x), 0.5 *(p1.y + p2.y));
    Segments diff(0.5 *(p2.x - p1.x), 0.5 * (p2.y - p1.y));

    double angle = degreesToRadians(angleDgrees);
	double cos1 = cos(angle);
	double sin1 = sin(angle);

    double x1 = cos1 * diff.x + sin1 * diff.y;
    double y1 = -sin1 * diff.x + cos1 * diff.y;

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

    double cx = mid.x + (cos1 * cx1 - sin1 * cy1);
    double cy = mid.y + (sin1 * cx1 + cos1 * cy1);

	double ux = (x1 - cx1) / rx;
	double uy = (y1 - cy1) / ry;
	double vx = (-x1 - cx1) / rx;
	double vy = (-y1 - cy1) / ry;
	double n = sqrt((ux * ux) + (uy * uy));
	double p = ux;
	sign = (uy < 0) ? -1 : 1;

	double theta = sign * acos(p / n);
    theta = radiansToDegree(theta);

	n = sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
	p = ux * vx + uy * vy;
	sign = ((ux * vy - uy * vx) < 0) ? -1 : 1;
	double delta = sign * acos(p / n);
    delta = radiansToDegree(delta);

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

double GeometryUtil::degreesToRadians(double angle)
{
	return angle * (PI / 180);
}

double GeometryUtil::radiansToDegree(double angle)
{
	return angle * (180 / PI);
}

bool GeometryUtil::withinDistance(Segments p1, Segments p2, double distance)
{
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
	return ((dx * dx + dy * dy) < distance * distance);
}

