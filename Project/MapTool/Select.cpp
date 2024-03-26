#include "Select.h"
#include"CInput.h"
#include"CoreInterface.h"
void Select::SetMatrix(TMatrix* pWorld, TMatrix* pView, TMatrix* pProj)
{
	
		if (pWorld != NULL)
		{
			m_matWorld = *pWorld;
			m_vCenter.x = pWorld->_41;
			m_vCenter.y = pWorld->_42;
			m_vCenter.z = pWorld->_43;
		}
		if (pView != NULL)
		{
			m_matView = *pView;
		}
		if (pProj != NULL)
		{
			m_matProj = *pProj;
		}
		Update();
	
}
void Select::Update()
{

	{
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(g_hWnd, &ptCursor);

		TVector3 v;

		v.x = (((2.0f * ptCursor.x) / gWindow_Width) - 1) / m_matProj._11;
		v.y = -(((2.0f * ptCursor.y) / gWindow_Height) - 1) / m_matProj._22;
		v.z = 1.0f;
		TMatrix mWorldView = m_matWorld * m_matView;
		TMatrix m;
		D3DXMatrixInverse(&m, NULL, &mWorldView);

		m_Ray.vOrigin = TVector3(0.0f, 0.0f, 0.0f);
		m_Ray.vDirection = TVector3(v.x, v.y, v.z);
		D3DXVec3TransformCoord(&m_Ray.vOrigin, &m_Ray.vOrigin, &m);
		D3DXVec3TransformNormal(&m_Ray.vDirection, &m_Ray.vDirection, &m);
		D3DXVec3Normalize(&m_Ray.vDirection, &m_Ray.vDirection);
		
		
	}
}


bool Select::ChkPick(TVector3& v0, TVector3& v1, TVector3& v2)
{
	FLOAT fBary1, fBary2;
	FLOAT fDist;
	if (IntersectTriangle(m_Ray.vOrigin, m_Ray.vDirection, v0, v1, v2,
		&fDist, &fBary1, &fBary2))
	{
		m_vSrcVex[0] = v0;
		m_vSrcVex[1] = v1;
		m_vSrcVex[2] = v2;
		m_matWorldPick = m_matWorld;
		return true;
	}

	return false;
}

bool Select::IntersectTriangle(const TVector3& orig, const TVector3& dir,
	TVector3& v0, TVector3& v1, TVector3& v2,
	FLOAT* t, FLOAT* u, FLOAT* v)
{
	TVector3 edge1 = v1 - v0;
	TVector3 edge2 = v2 - v0;

	D3DXVec3Cross(&pvec, &dir, &edge2);
	FLOAT det = D3DXVec3Dot(&edge1, &pvec);

	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return false;

	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return false;

	D3DXVec3Cross(&qvec, &tvec, &edge1);

	*v = D3DXVec3Dot(&dir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return false;

	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

bool Select::GetIntersectionBox(CBox* pBox, RAY* pRay)
{
	
	if (pRay == NULL) pRay = &m_Ray;
	float t_min = -999999.0f;
	float t_max = 999999.0f;
	float  f[3], fa[3], s[3], sa[3];

	TVector3 vR = pRay->vOrigin - pBox->m_pCenter;

	for (int v = 0; v < 3; v++)
	{
		f[v] = D3DXVec3Dot(&pBox->m_vAxis[v], &pRay->vDirection);
		s[v] = D3DXVec3Dot(&pBox->m_vAxis[v], &vR);
		fa[v] = fabs(f[v]);
		sa[v] = fabs(s[v]);

		if (sa[v] > pBox->m_fExtent[v] && s[v] * f[v] >= 0.0f)
			return false;

		float t1 = (-s[v] - pBox->m_fExtent[v]) / f[v];
		float t2 = (-s[v] + pBox->m_fExtent[v]) / f[v];
		if (t1 > t2)
		{
			swap(t1, t2);
		}
		t_min = max(t_min, t1);
		t_max = min(t_max, t2);
		if (t_min > t_max)
			return false;
	}

	float  fCross[3], fRhs;
	TVector3 vDxR;
	D3DXVec3Cross(&vDxR, &pRay->vDirection, &vR);
	// D X pBox->vAxis[0]  분리축
	fCross[0] = fabs(D3DXVec3Dot(&vDxR, &pBox->m_vAxis[0]));
	float fAxis2 = pBox->m_fExtent[1] * fa[2];
	float fAxis1 = pBox->m_fExtent[2] * fa[1];
	fRhs = fAxis2 + fAxis1;
	if (fCross[0] > fRhs)
	{
		m_vDxR = vDxR;
		return false;
	}
	// D x pBox->vAxis[1]  분리축
	fCross[1] = fabs(D3DXVec3Dot(&vDxR, &pBox->m_vAxis[1]));
	fRhs = pBox->m_fExtent[0] * fa[2] + pBox->m_fExtent[2] * fa[0];
	if (fCross[1] > fRhs)
	{
		m_vDxR = vDxR;
		return false;
	}
	// D x pBox->vAxis[2]  분리축
	fCross[2] = fabs(D3DXVec3Dot(&vDxR, &pBox->m_vAxis[2]));
	fRhs = pBox->m_fExtent[0] * fa[1] + pBox->m_fExtent[1] * fa[0];
	if (fCross[2] > fRhs)
	{
		m_vDxR = vDxR;
		return false;
	}

	m_vIntersection = pRay->vOrigin + pRay->vDirection * t_min;

	return true;
}

bool Select::GetSelectNode(CBox* pBox)
{
	Circle cir;
	cir.vCenter = { m_vIntersection.x,m_vIntersection.z };
	cir.fRadius = m_fPickDistance;

	return true;
}

