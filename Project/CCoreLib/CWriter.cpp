#include "CWriter.h"
#include"CInput.h"
bool CWriter::CreateWriteRV(IDXGISurface1* pSurface)
{
	float dpi = GetDpiForWindow(g_hWnd);


	D2D1_RENDER_TARGET_PROPERTIES RTProp;
	ZeroMemory(&RTProp, sizeof(RTProp));
	RTProp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	RTProp.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
	RTProp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	RTProp.dpiX = dpi;
	RTProp.dpiY = dpi;
	RTProp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	RTProp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	
	HRESULT hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(pSurface, &RTProp, m_pD2RenderTarget.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	hr = m_pD2RenderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0, 1), m_pSolidColorBrush.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}
bool CWriter::Create(IDXGISurface1* pBackBuffer)
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_pD2DFactory.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)m_pDWriteFactory.GetAddressOf());

		if (SUCCEEDED(hr))
		{

		     	m_pDWriteFactory->CreateTextFormat(L"°íµñ",
				nullptr,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				20.0f,
				L"ko-kr",
				m_pTextFormat.GetAddressOf());
			
		}
	}
	CreateResource(pBackBuffer);
	return false;

}
void CWriter::AddText(std::wstring text, float x, float y, D2D1::ColorF Color)
{
	TextData Data;
	Data.Text = text;
	Data.Layout = { x,y,gWindow_Width,gWindow_Height };
	Data.color = Color;

	m_TextList.push_back(Data);

}
bool CWriter::CreateResource(IDXGISurface1* pBackBuffer)
{
	if (pBackBuffer)
	{

		if (CreateWriteRV(pBackBuffer))
		{
			return true;

		}

	}

	return true;
}
bool CWriter::DeleteResource()
{
	m_pD2RenderTarget.Reset();
	m_pSolidColorBrush.Reset();
	return true;
}
bool CWriter::Init()
{

	return true;
}
bool CWriter::Frame()
{
	if (CInput::GetInstance().m_dwKeyState[VK_F4] == KEY_PUSH )
	{
		if(m_bValid)
		m_bValid = false;
		else
		m_bValid = true;
	}
	

	return true;
}
bool CWriter::PreRender()
{
	if (m_pD2RenderTarget&& m_bValid==true)
	{
		m_pD2RenderTarget->BeginDraw();
		m_pD2RenderTarget->SetTransform(D2D1::IdentityMatrix());
	}


	return true;
}
bool CWriter::Render()
{
	if (PreRender())
	{
		m_pTextFormat->GetFontSize();
		for (int text = 0; text < m_TextList.size(); ++text)
		{
			std::wstring msg = m_TextList[text].Text;
			D2D1_RECT_F layout = m_TextList[text].Layout;
			m_pSolidColorBrush->SetColor(m_TextList[text].color);

			m_pD2RenderTarget->DrawTextW(msg.c_str(), msg.size(), m_pTextFormat.Get(), &layout, m_pSolidColorBrush.Get());


		}

		PostRender();
	}
	
	return true;
}
bool CWriter::PostRender()
{
	m_pD2RenderTarget->EndDraw();
	m_TextList.clear();
	return true;
}
bool CWriter::Release()
{
	m_TextList.clear();

	return true;
}