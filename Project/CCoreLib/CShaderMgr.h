#pragma once
#include "CDefine.h"
#include"CoreInterface.h"
class CShader
{
public:

	ComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
	ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;
	ComPtr<ID3DBlob> m_vBb = nullptr;
	ComPtr<ID3DBlob> m_pBb = nullptr;
public:
	bool Load(ID3D11Device* pDevice, std::wstring FileName);
	bool CreatePixelShader(ID3D11Device* pDevice,std::wstring FileName);
	bool CreateVertexShader(ID3D11Device* pDevice,std::wstring FileName);
	bool Release();
	bool Apply(ID3D11DeviceContext* pImmediateContext, int iSlot) const
	{
		CoreInterface::g_pImmediateContext->VSSetShader(m_VertexShader.Get(), NULL, 0); //¡§¡°Ω¶¿Ã¥ı º≥¡§
		CoreInterface::g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0); //«»ºøΩ¶¿Ã¥ı º≥¡§
		return true;
	}

};


class CShaderMgr
{


public:
		ComPtr<ID3D11Device> m_pDevice = nullptr;
		ComPtr < ID3D11DeviceContext> m_pImmediateContext = nullptr;
		std::map < std::wstring, CShader*> m_List;

public:

		bool Set(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext);
		const CShader* Load(std::wstring FileName);
		const CShader* GetPtr(std::wstring FileName);
		bool Get(std::wstring FileName, CShader& texture);
		bool Release();
		static CShaderMgr& GetInstance()
		{
			static CShaderMgr Shader;
			return Shader;

		}


private:
	CShaderMgr();
public:

	virtual ~CShaderMgr();

};

