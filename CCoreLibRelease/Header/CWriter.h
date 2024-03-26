#pragma once
#include "CDefine.h"

#include <d2d1.h>
#include<d2d1helper.h>
#include<dwrite.h>

#pragma comment (lib,"d2d1.lib")
#pragma comment(lib, "dwrite.lib")

struct TextData
{
	std::wstring Text;
	D2D1_RECT_F  Layout;
	D2D1::ColorF color = { 0.0f, 0.0f, 0.0f, 1.0f };
};
class CWriter 
{
public:
	bool m_bValid = true;
	std::vector<TextData> m_TextList;
public:
	ComPtr<ID2D1Factory> m_pD2DFactory = nullptr;
	ComPtr<IDWriteFactory> m_pDWriteFactory = nullptr;
	ComPtr<IDWriteTextFormat> m_pTextFormat = nullptr;
	ComPtr < ID2D1RenderTarget> m_pD2RenderTarget = nullptr;
	ComPtr < ID2D1SolidColorBrush> m_pSolidColorBrush = nullptr;
public:
	bool Create(IDXGISurface1* pBackBuffer);
	void AddText(std::wstring text,float x,float y,D2D1::ColorF Color);
	virtual bool CreateResource(IDXGISurface1* pBackBuffer);
	virtual bool DeleteResource();
	bool CreateWriteRV(IDXGISurface1* pSurface);
	static CWriter& GetInstance()
	{
		static CWriter Instance;
		return Instance;
	}
public:

	bool Init();
	bool Frame();
	bool PreRender();
	bool Render();
	bool PostRender();
	bool Release();
private:
	CWriter() {};

};

