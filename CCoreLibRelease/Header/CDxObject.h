#pragma once
#include"CDefine.h"
#include"CTextureMgr.h"
#include "CShaderMgr.h"



//struct PNCT_VERTEX
//{
//
//	TVector3 Pos;
//	TVector3 Nor;
//	TVector4 Col;
//	TVector2 Tex;
//
//
//};
struct ConstantData
{
	TMatrix  WolrdMatrix;
	TMatrix  ViewMatrix;
	TMatrix  ProjMatrix;
	TVector4   MeshColor = TVector4(1.0f,1.0f,1.0f,1.0f);
};
struct HeightCBData
{
	TMatrix  WolrdMatrix;
	TMatrix  ViewMatrix;
	TMatrix  ProjMatrix;
	TVector4 Alpha;
};
class CDxObject
{

public:
	
	ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_pIndexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_pConstantBuffer = nullptr;
	ComPtr<ID3D11InputLayout> m_pInputLayout = nullptr;
	const CTexture* m_pTexture = nullptr;
	const CShader* m_pShader = nullptr;
	ConstantData m_ConstantData;
	std::vector<PNCT_VERTEX> m_vVertexList;
	std::vector<DWORD> m_vIndexList;

public:
	virtual bool CreateVertexBuffer();
	virtual bool CreateConstantBuffer();
	virtual bool CreateInputLayout();
	virtual bool CreateIndexBuffer();
	virtual bool Set(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext) { return true; };
public:
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();
	virtual bool Release();
public:
	virtual ~CDxObject() {};


};

