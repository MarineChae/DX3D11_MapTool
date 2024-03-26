#pragma once
#include<string>
#include"CObject.h"
struct CMapDesc
{
	int iCol;
	int iRow;
	float fCellDistance;
	float fScaleHeight;
	std::wstring szTextureName;
	std::wstring szShaderName;


};

class CMap : public CObject
{
public:
	CMapDesc m_MapDesc;
	int m_iCol;
	int m_iRow;
	float m_fCellDistance;
	int m_iCellRow;
	int m_iCellCol;
	int m_iVertices;
	int m_iNumFace;
	bool m_bStaticLight;
	TVector3 m_vLightDir;
	TVector3* m_pFaceNormal;
	int m_iTexIndex;
	HeightCBData m_ConstHeightData;
	TVector3* m_pFaceNormals;
	int* m_pNormalLookupTable;
	TMatrix				m_matWorld;
	TMatrix				m_matView;
	TMatrix				m_matProj;
	TMatrix				m_matNormal;
public:
	BYTE* m_fLookup =nullptr;
	ComPtr<ID3D11Texture2D>				m_pRoughnessLookUpTex;
	ComPtr<ID3D11ShaderResourceView>	m_pLookupRV;
public:
	virtual  bool Load(CMapDesc& MapDesc, std::vector<float> heightlist);
	 virtual bool CreateMap(CMapDesc& MapDesc, std::vector<float> heightlist);
	virtual bool CreateVertexData();
	virtual bool CreateIndexData();
	virtual float GetHeightVertex(UINT index);
	virtual TVector3 GetNormalVertex(UINT index);
	virtual TVector4 GetColorVertex(UINT index);
	virtual bool GenerateVertexNormal();
	void CalcVertexColor(TVector3 vLightDir);
	virtual void InitFaceNormal();
	//bool CreateVertexBuffer();
	//bool CreateIndexBuffer();
	virtual void GenNormalLookupTable();
	virtual void CalcPerVertexNormalsFastLookup();
	virtual void CalcFaceNormals();
	virtual TVector3 ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2);
	void SetMatrix(TMatrix* WolrdMatrix, TMatrix* ViewMatrix, TMatrix* ProjMatrix);
	HRESULT CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight);

	bool Init();
	bool Frame();

	bool Release();
public:
	CMap(void);
};

