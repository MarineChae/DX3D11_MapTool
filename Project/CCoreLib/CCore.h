#pragma once
#include"CDevice.h"
#include"CTimer.h"
#include"CInput.h"
#include"CCamera.h"
#include"CWriter.h"

class CCore : public CDevice
{
protected:
	CTimer		 m_GameTimer;
	std::shared_ptr<CCamera> m_pDefaultCamera=nullptr;

	ComPtr<ID3D11BlendState> m_pBlendState;
	ComPtr<ID3D11SamplerState> m_pSamplerState;

	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	ComPtr<ID3D11RasterizerState> m_pRasterizerSolid;
	ComPtr<ID3D11RasterizerState> m_pRasterizerWire;
public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
	bool Run();
	bool CreateBlendState();
	void CreateSamplerState();

	void CreateDepthStencilState();
	void SetRasterizerState();
	void ResizeDevice(UINT width, UINT height);
public:
	virtual bool DeleteResource();
	virtual bool CreateResource();
private:
	virtual bool EngineInit();
	virtual bool EngineFrame();
	virtual bool EngineRender();
	virtual bool EngineRelease();
public:
	CCore() {};
	virtual ~CCore() {};


};

