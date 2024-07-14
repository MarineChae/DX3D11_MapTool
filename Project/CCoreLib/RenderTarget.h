#pragma once
#include"CDefine.h"
class RenderTarget
{
public:
	ComPtr<ID3D11Texture2D>			 m_RTTexture;
	ComPtr<ID3D11RenderTargetView>   m_pRenderTargetView;
	ComPtr<ID3D11Texture2D>			 m_DSTexture;
	ComPtr<ID3D11DepthStencilView>	 m_pDepthStencilView;
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	ComPtr<ID3D11ShaderResourceView> m_pDSSRV;
	D3D11_VIEWPORT					 m_ViewPort;

	UINT	m_iTexWidth;
	UINT	m_iTexHeight;
	UINT    m_iNumTargetTextures;
	UINT	m_nViewPorts;
	//oldview
	D3D11_VIEWPORT					 m_vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	ComPtr<ID3D11RenderTargetView>   m_pOldRTV;
	ComPtr<ID3D11DepthStencilView>	 m_pOldDSV;
	DXGI_FORMAT m_DSFormat;
	D3D11_DEPTH_STENCIL_VIEW_DESC			m_DepthStencilDesc;
public:
	HRESULT Create(int iTexWidth, int iTexHeight);
	bool Begin(TVector4 clearColor, bool bTargetClear = true, bool bDepthClear = true, bool bStencilClear = true);
	bool Clear(TVector4 clearColor, bool bTargetClear = true, bool bDepthClear = true, bool bStencilClear = true);
	bool End();
	HRESULT UpdateDepthStencilView(ID3D11Device* pDevice, UINT Width, UINT Height);
public:
	RenderTarget()
	{
		m_pRenderTargetView = nullptr;
		m_pDepthStencilView = nullptr;
		m_pSRV = nullptr;
		m_RTTexture = nullptr;
		m_DSFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}
};

