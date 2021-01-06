#include "NfpUtil.h"
#include "GeometryUtil.h"
#include "qDebug"

ParallelData* NfpUtil::NfpGenerator(NfpPair pair, Config config)
{
//    qDebug() << "NfpGenerator" << endl;
//    qDebug() << "NfpPairA:" << pair.getA().getId()<<","<<pair.getA().getRotation();
//    qDebug() << "NfpPairB:" << pair.getB().getId()<<","<<pair.getB().getRotation();
//    qDebug() << "NfpPairKEY:" << pair.getKey().getA()<<","<<pair.getKey().getB();
//    qDebug() << "NfpPairKEY:" << pair.getKey().getARotation()<<","<<pair.getKey().getBRotation();
	bool searchEdges = config.isConcave();
    bool useHoles = config.isUseHole();

    NestPath A = GeometryUtil::rotatePolygon2Polygon(pair.getA(), pair.getKey().getARotation());
    NestPath B = GeometryUtil::rotatePolygon2Polygon(pair.getB(), pair.getKey().getBRotation());

    qDebug() << "nfp" << endl;
	vector<NestPath> *nfp = new vector<NestPath>();
    if (pair.getKey().isInside())
	{
        qDebug() << "pair.getKey().isInside" << endl;
        if (GeometryUtil::isRectangle(A, 0.001))
		{
            nfp = GeometryUtil::noFitPolygonRectangle(A, B);
			if (nfp == NULL) 
			{
                qDebug() << "nfp == NULL" << endl;
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
            qDebug() << "nfp == NULL || nfp->size() > 0" << endl;
			//Warning on null inner NFP
		}
	}
	else 
	{
        qDebug() << "pair.getKey().isInside22" << endl;
		int count = 0;
		if (searchEdges) 
		{

			// NFP Generator TODO  double scale contorl
            *nfp = GeometryUtil::noFitPolygon(A, B, false, searchEdges);
			if (nfp == NULL) 
			{
                qDebug() << "nfp == NULL2" << endl;
			}
		}
		else 
		{

            *nfp = GeometryUtil::minkowskiDifference(A, B);
		}
		// sanity check
		if (nfp == NULL || nfp->size() == 0) 
		{
            qDebug() << "nfp == NULL3" << endl;
			return NULL;
		}
		for (int i = 0; i < nfp->size(); i++)
		{
			if (!searchEdges || i == 0) 
			{
                if (abs(GeometryUtil::polygonArea(nfp->at(i))) < abs(GeometryUtil::polygonArea(A)))
				{
					nfp->erase(nfp->begin() + i);
                    qDebug() << "nfp == NULL4" << endl;
					return NULL;
				}
			}
		}
		if (nfp->size() == 0)
		{
            qDebug() << "nfp == NULL5" << endl;
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
        qDebug() << "nfp is null" << endl;
	}
    qDebug() << "return new ParallelData" << endl;
    return new ParallelData(pair.getKey(), nfp);
}
