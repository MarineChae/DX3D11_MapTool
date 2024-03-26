#pragma once
#include"CDefine.h"
#include"CNode.h"
#include<set>

struct RAY
{
	float			fExtent;
	TVector3		vOrigin;
	TVector3		vDirection;
	RAY()
	{
		//  fExtent > 0 : 세그먼트로 사용 
		fExtent = -1;
	}
};
class Select
{
public:
	RAY		    m_Ray;
	TVector3	m_vIntersection;
	TMatrix		m_matWorld;
	TMatrix		m_matView;
	TMatrix		m_matProj;
	TVector3	m_vDxR;
	TVector3	m_vCenter;
	TVector3	m_vSrcVex[3];
	TVector3    m_vCircleCenter;
	float		m_fPickDistance=5;
	TMatrix		m_matWorldPick;
	TVector3	qvec;
	TVector3	tvec;
	TVector3	pvec;
	std::set<CNode*>m_vSelectNodeList;

public:
	void SetMatrix(TMatrix* pWorld, TMatrix* pView, TMatrix* pProj);
	void Update();
	bool ChkPick(TVector3& v0, TVector3& v1, TVector3& v2);
	bool IntersectTriangle(const TVector3& orig, const TVector3& dir,
						   TVector3& v0, TVector3& v1, TVector3& v2,
						   FLOAT* t, FLOAT* u, FLOAT* v);
	bool GetIntersectionBox(CBox* pBox, RAY* pRay = NULL);
	bool GetSelectNode(CBox* pBox);
	bool InterpolationNodeHeight();
};

