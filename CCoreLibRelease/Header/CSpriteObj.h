#pragma once
#include"CPlaneObj.h"
#include"CInput.h"
#include"CParticle.h"
struct CSpriteData
{
	TVector3 m_vPos;
	TVector3 m_vScale;
	std::wstring FileName;
	std::wstring ShaderName;
	int iCol=0;
	int iRow=0;
	int MaxUvSize=0;
	double m_fDelay = 0.0f;
	std::vector<std::wstring> m_TextureList;
	
};

class CSpriteObj :public CPlaneObj
{
public:
	
	int m_iTexIndex = 0;
	double m_AccumulatedTime = 0.0;
	CSpriteData SpriteInfo;
	

public:
	bool Load(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext, CSpriteData data);
	virtual int GetSize() { return 0; };
	virtual bool LoadTexData(std::vector<std::wstring> TexList);
	virtual bool SetUVData(int iNumRow, int iNumColumn);
public:


public:
	CSpriteObj() {};

	virtual ~CSpriteObj() {};




};


class CSpriteTexObj : public CSpriteObj
{
public:
	std::vector<const CTexture*> m_pTextureList;


public:
	//bool Load(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext, CSpriteData data);
	virtual bool LoadTexData(std::vector<std::wstring> TexList);
public:
	virtual int GetSize() { return m_pTextureList.size(); };
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
public:
	CSpriteTexObj();
	
	virtual ~CSpriteTexObj();
	
};

class CSpriteUVObj : public CSpriteObj
{
public:
	std::vector<CUVRect> m_pUVList;
	double m_SustainmentTime;

public:
	//bool Load(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext, CSpriteData data);
	virtual bool SetUVData(int iNumRow, int iNumColumn);
public:
	virtual int GetSize() { return m_pUVList.size(); };
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render(bool Face);
	virtual bool Release();
public:
	CSpriteUVObj();

	virtual ~CSpriteUVObj();

};
