#pragma once
#include "CObject.h"


class SkyBox :public CObject
{
public:

	ComPtr<ID3D11Texture2D>				m_pSkyTexture;
	ComPtr<ID3D11ShaderResourceView>    m_pSkyTexRV[6];
	ComPtr<ID3D11SamplerState>          m_pSkyBoxSamplerState;
public:
	bool Create(wstring pLoadShaderFile, wstring pLoadTextureString);
	void CreateCube();
	HRESULT CreateTextureArray();
	HRESULT LoadTextureArray(const TCHAR** szTextureName, int iNumTex, ID3D11Texture2D** ppTex2D, ID3D11ShaderResourceView** ppSRV);
	void CreateSamplerState();
public:
	bool Init();
	bool PostRender() override;

};

