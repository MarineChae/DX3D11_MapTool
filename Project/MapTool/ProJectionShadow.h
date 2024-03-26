#pragma once
#include "CDefine.h"
#include "CQuadTree.h"
class MapObject;
class RenderTarget;
class InstanceObject;
class ProJectionShadow
{
public:

	TVector3								m_vLightVector;
	TMatrix									m_matWolrd = TMatrix::Identity;
	TMatrix									m_matShadow;
	TMatrix									m_matViewLight;
	TMatrix									m_matProjLight;
	TMatrix									m_matTexture;
	TVector3								m_vLightPos;
	ComPtr<ID3D11DepthStencilState>			m_pDSS;
	ComPtr<ID3D11PixelShader>				m_pPixelShader;
	ComPtr<ID3D11VertexShader>				m_pVertexShader;
	SHADOW_CONSTANT_BUFFER			m_cbShadow;
	ComPtr<ID3D11Buffer>			m_pShadowCB;
	ComPtr<ID3D11SamplerState>		m_pShadowSS;

public:
	bool Init();
	bool Frame();
	bool Render(RenderTarget rt,map<UINT, shared_ptr<InstanceObject>> list, shared_ptr<CQuadTree> tree);
public:
	void CreateSamplerState();
	void RenderShadow(TMatrix* matWorld, TMatrix* matShadow, TMatrix* matView, TMatrix* matProj, MapObject* obj);

};

