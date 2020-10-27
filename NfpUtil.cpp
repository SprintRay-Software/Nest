#include "NfpUtil.h"
#include "GeometryUtil.h"


ParallelData* NfpUtil::NfpGenerator(NfpPair pair, Config config)
{
	bool searchEdges = config.isConcave();
    bool useHoles = config.isUseHole();

    NestPath A = GeometryUtil::rotatePolygon2Polygon(pair.getA(), pair.getKey().getARotation());
    NestPath B = GeometryUtil::rotatePolygon2Polygon(pair.getB(), pair.getKey().getBRotation());

	vector<NestPath> *nfp = new vector<NestPath>();
    if (pair.getKey().isInside())
	{
        if (GeometryUtil::isRectangle(A, 0.001))
		{
            nfp = GeometryUtil::noFitPolygonRectangle(A, B);
			if (nfp == NULL) 
			{

			}
		}
		else 
		{
            *nfp = GeometryUtil::noFitPolygon(A, B, true, searchEdges);
		}
		if (nfp != NULL && nfp->size() > 0)
		{
			for (int i = 0; i < nfp->size(); i++)
			{
                if (GeometryUtil::polygonArea(nfp->at(i)) > 0)
				{
                    nfp->at(i).reverse();
				}
			}
		}
		else 
		{
			//Warning on null inner NFP
		}
	}
	else 
	{
		int count = 0;
		if (searchEdges) 
		{

			// NFP Generator TODO  double scale contorl
            *nfp = GeometryUtil::noFitPolygon(A, B, false, searchEdges);
			if (nfp == NULL) 
			{

			}
		}
		else 
		{

            *nfp = GeometryUtil::minkowskiDifference(A, B);
		}
		// sanity check
		if (nfp == NULL || nfp->size() == 0) 
		{

			return NULL;
		}
		for (int i = 0; i < nfp->size(); i++)
		{
			if (!searchEdges || i == 0) 
			{
                if (abs(GeometryUtil::polygonArea(nfp->at(i))) < abs(GeometryUtil::polygonArea(A)))
				{
					nfp->erase(nfp->begin() + i);

					return NULL;
				}
			}
		}
		if (nfp->size() == 0)
		{

			return NULL;
		}

		for (int i = 0; i < nfp->size(); i++)
		{
            if (GeometryUtil::polygonArea(nfp->at(i)) > 0)
			{
                nfp->at(i).reverse();
			}

			if (i > 0) 
			{
                if (GeometryUtil::pointInPolygon(nfp->at(i).Get(0), nfp->at(0)))
				{
                    if (GeometryUtil::polygonArea(nfp->at(i)) < 0)
					{
                        nfp->at(i).reverse();
					}
				}
			}
		}

        if (useHoles && A.getChildren()->size() > 0)
		{
            Bound bBounds = GeometryUtil::getPolygonBounds(B);
            for (int i = 0; i < A.getChildren()->size(); i++)
			{
                Bound aBounds = GeometryUtil::getPolygonBounds(A.getChildren()->at(i));

                if (aBounds.width > bBounds.width && aBounds.height > bBounds.height)
				{

                    vector<NestPath> cnfp = GeometryUtil::noFitPolygon(A.getChildren()->at(i), B, true, searchEdges);
					// ensure all interior NFPs have the same winding direction

					if (cnfp.size() > 0) 
					{

						for (int j = 0; j < cnfp.size(); j++) 
						{
                            if (GeometryUtil::polygonArea(cnfp.at(j)) < 0)
							{
                                cnfp.at(j).reverse();
							}
							nfp->push_back(cnfp.at(j));
						}
					}

				}
			}
		}
	}
	if (nfp == NULL) 
	{

	}
    return new ParallelData(pair.getKey(), nfp);
}
