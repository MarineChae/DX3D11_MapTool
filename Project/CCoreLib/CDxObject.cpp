#include "CDxObject.h"
#include"CoreInterface.h"
bool CDxObject::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC Desc;
   	Desc.ByteWidth = sizeof(PNCT_VERTEX) * m_vVertexList.size();
   	Desc.Usage = D3D11_USAGE_DEFAULT;
   	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   	Desc.CPUAccessFlags = 0;
   	Desc.MiscFlags = 0;
   	Desc.StructureByteStride = 0;
   	D3D11_SUBRESOURCE_DATA InitialData;
   	InitialData.pSysMem = &m_vVertexList.at(0);
   
   
   	CoreInterface::g_pDevice->CreateBuffer(
   		&Desc,
   		&InitialData,
   		m_pVertexBuffer.GetAddressOf());
   

    return true;
}

bool CDxObject::CreateConstantBuffer()
{

    D3D11_BUFFER_DESC Desc;
    Desc.ByteWidth = sizeof(ConstantData);
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
    Desc.StructureByteStride = 0;


    CoreInterface::g_pDevice->CreateBuffer(
        &Desc,
        nullptr,
        m_pConstantBuffer.GetAddressOf());


    return true;
}


bool CDxObject::CreateInputLayout()
{

    D3D11_INPUT_ELEMENT_DESC InputElementDescs[] =
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},

        {"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"EMISSIVE",0,DXGI_FORMAT_R32_FLOAT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    HRESULT hr = CoreInterface::g_pDevice->CreateInputLayout(
        InputElementDescs,
        sizeof(InputElementDescs) / sizeof(D3D11_INPUT_ELEMENT_DESC),
        m_pShader->m_vBb->GetBufferPointer(),
        m_pShader->m_vBb->GetBufferSize(),
        m_pInputLayout.GetAddressOf());
    if ((hr)) 
    {
        LPCSTR errorText = "nullptr";
        errorText = (LPCSTR)hr;

        MessageBoxA(NULL, errorText, "Error", MB_OK | MB_ICONERROR);
    }
    return true;
}

bool CDxObject::CreateIndexBuffer()
{
    if (m_vIndexList.size() == 0)return false;
  	D3D11_BUFFER_DESC Desc;
  	Desc.ByteWidth = sizeof(DWORD)*m_vIndexList.size();//버퍼의 크기
  	Desc.Usage = D3D11_USAGE_DEFAULT;//버퍼를 어디에서 읽을것인지
  	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//버퍼의 사용용도
  	Desc.CPUAccessFlags = 0;//cpu의 엑세스유형 필요없으면 null
  	Desc.MiscFlags = 0;//추가적인 옵션
  	Desc.StructureByteStride = 0;
  	D3D11_SUBRESOURCE_DATA pInitialData;
  	pInitialData.pSysMem = &m_vIndexList.at(0);
  
  	CoreInterface::g_pDevice->CreateBuffer(&Desc, &pInitialData, m_pIndexBuffer.GetAddressOf());
    return true;

}





bool CDxObject::PreRender()
{
    CoreInterface::g_pImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
    CoreInterface::g_pImmediateContext->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
    if (m_pTexture)
    {
        m_pTexture->Apply(CoreInterface::g_pImmediateContext.Get(), 0);
    }

    CoreInterface::g_pImmediateContext->IASetInputLayout(m_pInputLayout.Get());      //

    if (m_pShader)
    {
        m_pShader->Apply(CoreInterface::g_pImmediateContext.Get(), 0);
    }
    CoreInterface::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 받아온 데이터를 어떤 방식으로 해석할지

    UINT Stride = sizeof(PNCT_VERTEX);
    UINT offset = 0;
    CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Stride, &offset);//p 159참조
    
    return true;
}

bool CDxObject::Render()
{
    PreRender();
    PostRender();
   
    return true;
}

bool CDxObject::PostRender()
{
    if (m_pIndexBuffer == nullptr)
    {
        CoreInterface::g_pImmediateContext->Draw(m_vVertexList.size(), 0);
    }
    else
    {
        CoreInterface::g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        CoreInterface::g_pImmediateContext->DrawIndexed(m_vIndexList.size(), 0, 0);
    }
  

   
    return true;
}


bool CDxObject::Release()
{

    m_vVertexList.clear();
    m_vIndexList.clear();
    return true;
}
