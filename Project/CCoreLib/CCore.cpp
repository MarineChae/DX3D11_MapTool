#include "CCore.h"
#include"CShaderMgr.h"
#include"CTextureMgr.h"
#include"CoreInterface.h"
#include"CSoundMgr.h"
#include"ImGuiManager.h"

bool CCore::Init()
{
	return true;
}

bool CCore::Frame()
{
	return true;
}


bool CCore::Render()
{
	return true;
}


bool CCore::Release()
{
	return true;
}

bool CCore::EngineInit()
{  


    CDevice::Init();
    CoreInterface::g_pDevice = m_pDevice;
    CoreInterface::g_pImmediateContext = m_pImmediateContext;

    m_GameTimer.Init();
   // CreateBlendState();
    CreateSamplerState();
    CreateDepthStencilState();
    SetRasterizerState();

    CInput::GetInstance().Init();
    m_pDefaultCamera = std::make_shared<CCamera>();
    m_pDefaultCamera->Init();
    TVector3 vPos = { 0,0,-10 };
    TVector3 vUp = { 0,0,0 };
    m_pDefaultCamera->PerspectiveFovLH(
        PI * 0.25, (float)gWindow_Width / (float)gWindow_Height,
        1.0f, 1000.0f);
    CoreInterface::g_pMainCamera = m_pDefaultCamera;

    CShaderMgr::GetInstance().Set(m_pDevice.Get(),m_pImmediateContext.Get());
    CTextureMgr::GetInstance().Set(m_pDevice.Get(), m_pImmediateContext.Get());
    ImGuiManager::GetInstance().Init();

    CWriter::GetInstance().Init();
    if (m_pSwapChain)
    {
        IDXGISurface1* BackBuffer;
        HRESULT HR = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (LPVOID*)&BackBuffer);
        //반환되는 인터페이스 타입 지정
        if (SUCCEEDED(HR))
        {
            CWriter::GetInstance().Create(BackBuffer);
           
             BackBuffer->Release();
         
           
        }
    }
   
    Init();
    return true;
}

bool CCore::EngineFrame()
{
    
    m_GameTimer.Frame();
    CWriter::GetInstance().Frame();
    CInput::GetInstance().Frame();
    CoreInterface::g_pMainCamera->Frame();
    CSoundMgr::GetInstance().Frame();
    CDevice::Frame();
    ImGuiManager::GetInstance().Frame();
    Frame();


    return true;
}

bool CCore::EngineRender()
{
    CDevice::PreRender();
    m_pImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
    m_pImmediateContext->OMSetBlendState(m_pBlendState.Get(), 0, -1);
    m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1);
    m_pImmediateContext->RSSetState(m_pRasterizerSolid.Get());
    if (CInput::GetInstance().m_dwKeyState['V'] == KEY_HOLD)
    {
        m_pImmediateContext->RSSetState(m_pRasterizerWire.Get());
    }
   

    Render();
    CInput::GetInstance().Render();
    m_GameTimer.Render();
    CWriter::GetInstance().Render();
    CoreInterface::g_pMainCamera->Render();
    ImGuiManager::GetInstance().Render();
   

   
    CDevice::PostRender();
    return true;
}

bool CCore::EngineRelease()
{
    m_GameTimer.Release();
    Release();
    CWriter::GetInstance().Release();
    CoreInterface::g_pMainCamera->Release();
    m_pDefaultCamera->Release();
    
    CSoundMgr::GetInstance().Release();
    CTextureMgr::GetInstance().Release();
    CInput::GetInstance().Release();
    CDevice::Release();
   
    
    
    return true;
}




bool CCore::Run()
{
    EngineInit();

    MSG msg = { 0, };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            
            if(!EngineFrame())break;
            if(!EngineRender())break;
            //게임로직 처리
        }


    }

    EngineRelease();
   
    return true;
}

bool CCore::CreateBlendState()
{
    
        D3D11_BLEND_DESC BlendStateDesc;
        ZeroMemory(&BlendStateDesc, sizeof(BlendStateDesc));
        BlendStateDesc.RenderTarget[0].BlendEnable = true;

        BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        m_pDevice->CreateBlendState(&BlendStateDesc, m_pBlendState.GetAddressOf());


       
        return true;
    
 
}

void CCore::CreateSamplerState()
{
    D3D11_SAMPLER_DESC descSamp;
    descSamp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.MipLODBias = 0;
    descSamp.MaxAnisotropy = 16;

    descSamp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    descSamp.ComparisonFunc = D3D11_COMPARISON_NEVER;

    descSamp.BorderColor[0] = 1.0f;
    descSamp.BorderColor[1] = 0.0f;
    descSamp.BorderColor[2] = 0.0f;
    descSamp.BorderColor[3] = 1.0f;
    descSamp.MinLOD = 0;
    descSamp.MaxLOD = D3D11_FLOAT32_MAX;
    m_pDevice->CreateSamplerState(&descSamp, m_pSamplerState.GetAddressOf());

   

}


void CCore::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DSVDesc;
    ZeroMemory(&DSVDesc,sizeof(D3D11_DEPTH_STENCIL_DESC));
    DSVDesc.DepthEnable = TRUE;
    DSVDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DSVDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;


    DSVDesc.StencilEnable = FALSE;
    DSVDesc.StencilReadMask = 1;
    DSVDesc.StencilWriteMask = 1;
    DSVDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    DSVDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    DSVDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DSVDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    DSVDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    DSVDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DSVDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DSVDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    m_pDevice->CreateDepthStencilState(&DSVDesc, m_pDepthStencilState.GetAddressOf());






}

void CCore::SetRasterizerState()
{
    D3D11_RASTERIZER_DESC rsdesc;
    ZeroMemory(&rsdesc, sizeof(rsdesc));
    
    rsdesc.CullMode = D3D11_CULL_NONE;
    rsdesc.FillMode = D3D11_FILL_SOLID;
    m_pDevice->CreateRasterizerState(&rsdesc,m_pRasterizerSolid.GetAddressOf());

    rsdesc.FillMode = D3D11_FILL_WIREFRAME;
    m_pDevice->CreateRasterizerState(&rsdesc, m_pRasterizerWire.GetAddressOf());
    
}

void CCore::ResizeDevice(UINT width, UINT height)
{
    if (m_pDevice == nullptr) return;
    DeleteResource();
    m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
    m_DefaultRT.m_pRenderTargetView->Release();
    m_DefaultRT.m_pDepthStencilView->Release();
    HRESULT hr;

    hr= m_pSwapChain->ResizeBuffers(m_SwapChainDesc.BufferCount, width, height,
                                    m_SwapChainDesc.BufferDesc.Format, m_SwapChainDesc.Flags);

    m_pSwapChain->GetDesc(&m_SwapChainDesc);

    SetRenderTarGetView();
    CreateDepthStencilView();
    SetViewPort();

    GetClientRect(m_hWnd, &m_ClientRect);
    gWindow_Width=m_dwWindowWidth  = m_ClientRect.right;
    gWindow_Height=m_dwWindowHeight = m_ClientRect.bottom;
    CreateResource();
   
}

bool CCore::DeleteResource()
{
    CWriter::GetInstance().DeleteResource();
    return true;
}

bool CCore::CreateResource()
{
    if (m_pSwapChain)
    {
        IDXGISurface1* BackBuffer;
        HRESULT HR = m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (LPVOID*)&BackBuffer);
        //반환되는 인터페이스 타입 지정
        if (SUCCEEDED(HR))
        {
            CWriter::GetInstance().CreateResource(BackBuffer);
            
            BackBuffer->Release();
        }
    }
   
    return true;
}

