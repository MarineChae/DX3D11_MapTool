#include "RenderTarget.h"
#include"CoreInterface.h"
HRESULT RenderTarget::Create(int iTexWidth, int iTexHeight)
{
	HRESULT hr = S_OK;
	m_iTexWidth = iTexWidth;
	m_iTexHeight = iTexHeight;

	m_ViewPort.Width = m_iTexWidth;
	m_ViewPort.Height = m_iTexHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	CoreInterface::g_pImmediateContext->RSSetViewports(1, &m_ViewPort);

	
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = m_iTexWidth;
	texDesc.Height = m_iTexHeight;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	hr = CoreInterface::g_pDevice.Get()->CreateTexture2D(&texDesc, NULL, &m_RTTexture);
	if (FAILED(hr))
		return hr;
	
	hr = CoreInterface::g_pDevice.Get()->CreateShaderResourceView(m_RTTexture.Get(), NULL, m_pSRV.GetAddressOf());
	if (FAILED(hr))
		return hr;

	hr = CoreInterface::g_pDevice.Get()->CreateRenderTargetView(m_RTTexture.Get(), NULL, m_pRenderTargetView.GetAddressOf());
	if (FAILED(hr))
		return hr;
	
	if (FAILED(hr = UpdateDepthStencilView(CoreInterface::g_pDevice.Get(), (UINT)iTexWidth, (UINT)iTexHeight)))
	{
		return hr;
	}



	return hr;
}

bool RenderTarget::Begin(TVector4 clearColor, bool bTargetClear, bool bDepthClear, bool bStencilClear)
{
	m_nViewPorts = 1;
	CoreInterface::g_pImmediateContext->RSGetViewports(&m_nViewPorts, m_vpOld);
	CoreInterface::g_pImmediateContext->OMGetRenderTargets(1, m_pOldRTV.GetAddressOf(), m_pOldDSV.GetAddressOf());

	ComPtr<ID3D11RenderTargetView> pNullRTV = NULL;
	ComPtr<ID3D11DepthStencilView> pNUllDSV = NULL;
	CoreInterface::g_pImmediateContext->OMSetRenderTargets(1, pNullRTV.GetAddressOf(), pNUllDSV.Get());


	CoreInterface::g_pImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
	
	Clear(clearColor,bTargetClear,bDepthClear,bStencilClear);
	CoreInterface::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CoreInterface::g_pImmediateContext->RSSetViewports(1, &m_ViewPort);


	return true;
}

bool RenderTarget::Clear(TVector4 clearColor, bool bTargetClear, bool bDepthClear, bool bStencilClear)
{

	if (bTargetClear)
	{
		const FLOAT color[] = { clearColor.x,clearColor.y,clearColor.z,clearColor.w };
		CoreInterface::g_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	}
	if (bDepthClear)
	{
		CoreInterface::g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	if (bStencilClear)
	{
		CoreInterface::g_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	CoreInterface::g_pImmediateContext->RSSetViewports(1, &m_ViewPort);


	return true;
}

bool RenderTarget::End()
{
	CoreInterface::g_pImmediateContext->RSSetViewports(m_nViewPorts, m_vpOld);
	CoreInterface::g_pImmediateContext->OMSetRenderTargets(1, m_pOldRTV.GetAddressOf(), m_pOldDSV.Get());

	if (m_pOldRTV)
		m_pOldRTV.Get()->Release();
	if (m_pOldDSV)
		m_pOldDSV.Get()->Release();


	return true;
}
HRESULT RenderTarget::UpdateDepthStencilView(ID3D11Device* pDevice, UINT Width, UINT Height)
{
	HRESULT hr;
	m_ViewPort.Width = (FLOAT)Width;
	m_ViewPort.Height = (FLOAT)Height;

	ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
	D3D11_TEXTURE2D_DESC DescDepth;
	ZeroMemory(&DescDepth, sizeof(D3D11_TEXTURE2D_DESC));
	DescDepth.Width = Width;
	DescDepth.Height = Height;
	DescDepth.MipLevels = 1;
	DescDepth.ArraySize = 1;
	if (m_DSFormat == DXGI_FORMAT_D24_UNORM_S8_UINT)
		DescDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	else
		DescDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	DescDepth.SampleDesc.Count = 1;
	DescDepth.SampleDesc.Quality = 0;
	DescDepth.CPUAccessFlags = 0;
	DescDepth.MiscFlags = 0;

	DescDepth.Usage = D3D11_USAGE_DEFAULT;
	// ¹é ¹öÆÛ ±íÀÌ ¹× ½ºÅÙ½Ç ¹öÆÛ »ý¼º
	DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (DescDepth.Format == DXGI_FORMAT_R24G8_TYPELESS || DescDepth.Format == DXGI_FORMAT_R32_TYPELESS)
	{
		// ±íÀÌ¸Ê Àü¿ë ±íÀÌ¸Ê »ý¼º
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	}


	
	if (FAILED(hr = pDevice->CreateTexture2D(&DescDepth, NULL, pDSTexture.GetAddressOf())))
	{
		return hr;
	}

	///// ½¦ÀÌ´õ ¸®¼Ò½º »ý¼º : ±íÀÌ ¸Ê ½¦µµ¿ì¿¡¼­ »ç¿ëÇÑ´Ù. ///
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	switch (DescDepth.Format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	}


	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	if (FAILED(hr = pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, m_pDepthStencilView.ReleaseAndGetAddressOf())))
	{
		return hr;
	}
	m_pDepthStencilView->GetDesc(&m_DepthStencilDesc);


	if (srvDesc.Format == DXGI_FORMAT_R32_FLOAT || srvDesc.Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		if (FAILED(hr = pDevice->CreateShaderResourceView(pDSTexture.Get(), &srvDesc, m_pDSSRV.ReleaseAndGetAddressOf())))
		{
			return hr;
		}
	}
	return hr;
}