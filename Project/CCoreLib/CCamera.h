#pragma once

#include"CFrustum.h"

class CCamera :public CFrustum
{
public:

	float				m_fOriginSpeed;
	float				m_fAspect;
	float				m_fFov;
	float				m_fNear;
	float				m_fFar;
public:
	TVector3   m_vLook;
	TVector3   m_vUp;
	TVector3   m_vSide;
	float      m_fSpeed = 10.0f;
public:
	TVector3			    m_vTargetPos;
	TVector3				m_vCameraPos;
	TMatrix					m_WorldMatrix;
	TMatrix					m_ViewMatrix;
	TMatrix					m_ProjMatrix;
	TMatrix					m_OrthProjectionMatrix;
	//float					Zoom = 0.5f;
	DWORD					m_dwWindowWidth;
	DWORD					m_dwWindowHeight;

public:
	bool CreateCamera(TVector3 Pos, TVector2 Size);
	TMatrix ViewLookAtLH(TVector3 vPos , TVector3 VTarget,TVector3 vUp);
	TMatrix PerspectiveFovLH(float NearDist, float FarDist , float Fov , float Aspect);
	void UpdateVector();
	TMatrix CreateProjMatrix(float fNear, float fFar, float fFov, float fAspect);
public:
	virtual bool Init();
	virtual bool Frame2D();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

};

