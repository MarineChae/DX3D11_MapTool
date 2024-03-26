#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include"CCamera.h"
class CoreInterface
{
public:
	static ComPtr<ID3D11Device> g_pDevice ;
	static ComPtr<ID3D11DeviceContext> g_pImmediateContext;
	static shared_ptr<CCamera> g_pMainCamera;
	
};

