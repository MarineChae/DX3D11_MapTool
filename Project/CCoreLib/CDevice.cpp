#include "CDevice.h"




bool CDevice::CreateDevice()
{
    HRESULT HR =D3D11CreateDevice(
        NULL,
        m_DriverType,
        NULL,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        &m_pFeatureLevels,
        1,
        D3D11_SDK_VERSION,
        m_pDevice.GetAddressOf(),
        NULL,
        m_pImmediateContext.GetAddressOf());
    if (FAILED(HR))
    {
        return false;
    }

    return true;
}

bool CDevice::CreateDepthStencilView()
{
    HRESULT hr;
    ComPtr<ID3D11Texture2D> DSVTex;

    DXGI_SWAP_CHAIN_DESC SCDesc;
    m_pSwapChain->GetDesc(&SCDesc);

    D3D11_TEXTURE2D_DESC TexDesc = { SCDesc.BufferDesc.Width,
                                     SCDesc.BufferDesc.Height,
                                     1,1,
                                     DXGI_FORMAT_R24G8_TYPELESS,
                                     1,0,
                                     D3D11_USAGE_DEFAULT,D3D11_BIND_DEPTH_STENCIL,
                                     0,0 };

    hr=m_pDevice->CreateTexture2D(&TexDesc, NULL, DSVTex.GetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
    ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DSVDesc.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D ;


    hr=m_pDevice->CreateDepthStencilView(DSVTex.Get(), &DSVDesc, m_DefaultRT.m_pDepthStencilView.GetAddressOf());
  


    return true;
}

bool CDevice::CreateFactory()
{
    HRESULT HR= CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)m_pFactory.GetAddressOf());
    if (FAILED(HR))
    {
        return false;
    }
   
    return true;
}

bool CDevice::CreateSwapChain()
{
    ;
    ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
    m_SwapChainDesc.BufferDesc.Width = m_dwWindowWidth;
    m_SwapChainDesc.BufferDesc.Height = m_dwWindowHeight;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;//주사율 60/1 형식 
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//컬러 출력형식 
    m_SwapChainDesc.SampleDesc.Count = 1; //안티엘리어싱 카운트와 퀄리티가 높을수록 그래픽은 좋아지지만 메모리를 많이 사용
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//버퍼의 사용용도
    m_SwapChainDesc.BufferCount = 1;
    m_SwapChainDesc.OutputWindow = m_hWnd;
    m_SwapChainDesc.Windowed = true;

    HRESULT HR = m_pFactory->CreateSwapChain( m_pDevice.Get(), &m_SwapChainDesc, m_pSwapChain.GetAddressOf());
    if (FAILED(HR))
    {
        return false;
    }

    


    return true;
}

bool CDevice::SetRenderTarGetView()
{
    ID3D11Texture2D* pBackBuffer;
    HRESULT HR = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    //반환되는 인터페이스 타입 지정
    if (FAILED(HR))
    {
        return false;
    }
    //백버퍼 얻기
    HR = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, m_DefaultRT.m_pRenderTargetView.GetAddressOf());
    //렌더링할 버퍼 2개를 생성 , 전면버퍼 후면버퍼
    if (FAILED(HR))
    {
        pBackBuffer->Release();
        return false;
    }
   
    //렌더할 버퍼를 지정
    pBackBuffer->Release();
    return true;
}

bool CDevice::SetViewPort()
{
    
    m_DefaultRT.m_ViewPort.TopLeftX = 0.0f;
    m_DefaultRT.m_ViewPort.TopLeftY = 0.0f;
    m_DefaultRT.m_ViewPort.Width= m_SwapChainDesc.BufferDesc.Width;
    m_DefaultRT.m_ViewPort.Height= m_SwapChainDesc.BufferDesc.Height;
    m_DefaultRT.m_ViewPort.MinDepth=0.0f;
    m_DefaultRT.m_ViewPort.MaxDepth=1.0f;
  

    return true;
}

bool CDevice::Init()
{
   
    CreateDevice();
    CreateFactory();
    CreateSwapChain();
    SetRenderTarGetView();
    SetViewPort();
    CreateDepthStencilView();
   
 
    return true;
}


bool CDevice::Frame()
{
    return true;
}

bool CDevice::PreRender()
{
    float color[4] = { 0.75f,0.35f,0.35f,1 }; //4번째 알파값은 투명도
    m_pImmediateContext->ClearRenderTargetView(m_DefaultRT.m_pRenderTargetView.Get(), color);// color의 색으로 버퍼를 지운다.
    m_pImmediateContext->ClearDepthStencilView(m_DefaultRT.m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);




    m_pImmediateContext->OMSetRenderTargets(1, m_DefaultRT.m_pRenderTargetView.GetAddressOf(), m_DefaultRT.m_pDepthStencilView.Get());
    m_pImmediateContext->RSSetViewports(1, &m_DefaultRT.m_ViewPort);
    return true;
}

bool CDevice::PostRender()
{
    m_pSwapChain->Present(0, 0);//백버퍼에서 렌더링한 이미지를 출력
    return true;
}


bool CDevice::Render()
{
   
   

    return true;
}




bool CDevice::Release()
{
  /*  if (m_pSwapChain)m_pSwapChain->Release();
    if (m_pDevice)m_pDevice->Release();
    if (m_pSwapChain)m_pImmediateContext->Release();
    if (m_pSwapChain) m_pRenderTargetView->Release();
    if (m_pFactory)m_pFactory->Release();

    m_pSwapChain = NULL;
    m_pDevice = NULL;
    m_pImmediateContext = NULL;
    m_pRenderTargetView = NULL;
    m_pFactory = NULL;*/
    return true;
}




