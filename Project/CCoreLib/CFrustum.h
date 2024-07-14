#pragma once
#include"CDefine.h"


class CFrustum 
{
public:
	CPlane	 m_Plane[6];
	TVector3 m_vFrustumPoint[8];

	

public:
	bool CreateRenderBox();
	bool CreateFrustum(TMatrix ViewMat, TMatrix ProjMat);
	bool ClassifyPoint(TVector3 v);
	bool CullingObject(CBox Box);
	void SetMatrix(TMatrix* pWorld, TMatrix* pView, TMatrix* pProj);
	BOOL ClassifyOBB(CBox* box);
	void ExtractPlanesD3D(CPlane* pPlanes, const TMatrix& comboMatrix);
	BOOL ClassifySphere(CSphere* sphere);
	
};

