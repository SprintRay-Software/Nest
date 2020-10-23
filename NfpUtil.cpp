#include "NfpUtil.h"
#include "GeometryUtil.h"


/**
	* 获取一对多边形，并生成nfp
	* @param pair
	* @param config
	* @return
	*/
ParallelData* NfpUtil::NfpGenerator(NfpPair pair, Config config)
{
	bool searchEdges = config.isConcave();
	bool useHoles = config.isUse_hole();

	NestPath A = GeometryUtil::RotatePolygon2Polygon(pair.GetA(), pair.GetKey().GetArotation());
	NestPath B = GeometryUtil::RotatePolygon2Polygon(pair.GetB(), pair.GetKey().GetBrotation());

	vector<NestPath> *nfp = new vector<NestPath>();
	if (pair.GetKey().isInside()) 
	{
		if (GeometryUtil::isRectangle(A, 0.001))
		{
			nfp = GeometryUtil::NoFitPolygonRectangle(A, B);
			if (nfp == NULL) 
			{

			}
		}
		else 
		{
			*nfp = GeometryUtil::NoFitPolygon(A, B, true, searchEdges);
		}
		if (nfp != NULL && nfp->size() > 0)
		{
			for (int i = 0; i < nfp->size(); i++)
			{
				if (GeometryUtil::PolygonArea(nfp->at(i)) > 0)
				{
					nfp->at(i).Reverse();
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
			*nfp = GeometryUtil::NoFitPolygon(A, B, false, searchEdges);
			if (nfp == NULL) 
			{

			}
		}
		else 
		{

			*nfp = GeometryUtil::MinkowskiDifference(A, B);
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
				if (abs(GeometryUtil::PolygonArea(nfp->at(i))) < abs(GeometryUtil::PolygonArea(A)))
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
			if (GeometryUtil::PolygonArea(nfp->at(i)) > 0)
			{
				nfp->at(i).Reverse();
			}

			if (i > 0) 
			{
				if (GeometryUtil::PointInPolygon(nfp->at(i).Get(0), nfp->at(0)))
				{
					if (GeometryUtil::PolygonArea(nfp->at(i)) < 0)
					{
						nfp->at(i).Reverse();
					}
				}
			}
		}

		if (useHoles && A.GetChildren()->size() > 0)
		{
			Bound Bbounds = GeometryUtil::GetPolygonBounds(B);
			for (int i = 0; i < A.GetChildren()->size(); i++)
			{
				Bound Abounds = GeometryUtil::GetPolygonBounds(A.GetChildren()->at(i));

				if (Abounds.m_width > Bbounds.m_width && Abounds.m_height > Bbounds.m_height)
				{

					vector<NestPath> cnfp = GeometryUtil::NoFitPolygon(A.GetChildren()->at(i), B, true, searchEdges);
					// ensure all interior NFPs have the same winding direction

					if (cnfp.size() > 0) 
					{

						for (int j = 0; j < cnfp.size(); j++) 
						{
							if (GeometryUtil::PolygonArea(cnfp.at(j)) < 0) 
							{
								cnfp.at(j).Reverse();
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
	return new ParallelData(pair.GetKey(), nfp);
}
