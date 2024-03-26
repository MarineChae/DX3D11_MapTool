#pragma once
#include "CMap.h"
#include"RenderTarget.h"

class WaterMap :public CMap
{
public:
	ComPtr<ID3D11ShaderResourceView> m_pReflectRV;
	ComPtr<ID3D11ShaderResourceView> m_pRefractRV;
	ComPtr<ID3D11ShaderResourceView> m_pNormalRV;
	ComPtr<ID3D11ShaderResourceView> m_pWaterTexRV;
	ComPtr<ID3D11RasterizerState>	 m_pRasterizerBackCullSolid;
	ComPtr<ID3D11RasterizerState>	 m_pRasterizerCullNoneSolid;
public:
	RenderTarget					m_WaterEffectRT[3];

public:
	bool Create(std::wstring FileName, std::wstring ShaderFileName)override;
	HRESULT LoadTextures(const wstring pLoadTextureString);
	TVector4 GetColorOfVertex(UINT Index);
	bool CreateRS();
	void DrawReflectMap(int num, TMatrix* world, TMatrix* view, TMatrix* proj);
public:
	bool Init();
	bool PreRender();
	bool Render(TMatrix* world , TMatrix* view , TMatrix* Proj);
	bool PostRender();
	bool Load(CMapDesc& MapDesc, std::vector<float> heightlist = {})override;


public:
	WaterMap();
	virtual ~WaterMap();
};

