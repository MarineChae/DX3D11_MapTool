#include "SkyBox.h"


bool SkyBox::Create(wstring pLoadShaderFile, wstring pLoadTextureString)
{
    CreateCube();
    m_pShader = CShaderMgr::GetInstance().Load(pLoadShaderFile);
    CObject::Create(pLoadTextureString, pLoadShaderFile);

    m_pTexture = CTextureMgr::GetInstance().Load(pLoadTextureString);
    CreateSamplerState();
	return true;
}

bool SkyBox::Init()
{
    Create(L"../../Resource/SkyBox.hlsl", L"");
    CreateTextureArray();
    return true;
}

bool SkyBox::PostRender()
{  
    CoreInterface::g_pImmediateContext->PSSetSamplers(0, 1, m_pSkyBoxSamplerState.GetAddressOf());

    CoreInterface::g_pImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    for (int iTex = 0; iTex < 6; iTex++)
    {
        CoreInterface::g_pImmediateContext->PSSetShaderResources(0, 1, m_pSkyTexRV[iTex].GetAddressOf());
        CoreInterface::g_pImmediateContext->DrawIndexed(6, 6*iTex, 0);
    }

    return true;
}

void SkyBox::CreateCube()
{
   
    m_vVertexList.resize(24);
   //m_vVertexList[0] = PNCT_VERTEX{ TVector3(-1000, -1000, -1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[1] = PNCT_VERTEX{ TVector3(-1000 , 1000, -1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[2] = PNCT_VERTEX{ TVector3( 1000,  1000, -1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[3] = PNCT_VERTEX{ TVector3( 1000, -1000, -1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //
   //m_vVertexList[4] = PNCT_VERTEX{ TVector3(-1000, -1000, 1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[5] = PNCT_VERTEX{ TVector3(-1000 , 1000, 1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[6] = PNCT_VERTEX{ TVector3(1000,  1000, 1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
   //m_vVertexList[7] = PNCT_VERTEX{ TVector3(1000, -1000, 1000),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    float w2 = 1000.0f;
    float h2 = 1000.0f;
    float d2 = 1000.0f;

    float w3 = -1000.0f;
    float h3 = -1000.0f;
    float d3 = -1000.0f;
    //// 앞면
    m_vVertexList[0] = PNCT_VERTEX{TVector3(w3, h3, d3),  TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,1.0f) };
    m_vVertexList[1] = PNCT_VERTEX{TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    m_vVertexList[2] = PNCT_VERTEX{TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(1.0f,0.0f) };
    m_vVertexList[3] = PNCT_VERTEX{TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(1.0f,1.0f) };
    // 뒷면 ->                                                         TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    m_vVertexList[4] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),  TVector2(1.0f, 1.0f) };
    m_vVertexList[5] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(0.0f, 1.0f) };
    m_vVertexList[6] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f, 0.0f) };
    m_vVertexList[7] = PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(1.0f, 0.0f) };
    // 윗면 ->
    m_vVertexList[8] = PNCT_VERTEX{ TVector3(w3, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),    TVector2(0.0f, 1.0f) };
    m_vVertexList[9] = PNCT_VERTEX{ TVector3(w3, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),   TVector2(0.0f, 0.0f) };
    m_vVertexList[10] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(1.0f, 0.0f) };
    m_vVertexList[11] = PNCT_VERTEX{ TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(1.0f, 1.0f) };
    // 아랫- >
    m_vVertexList[12] = PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),  TVector2(1.0f, 1.0f) };
    m_vVertexList[13] = PNCT_VERTEX{ TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(0.0f, 1.0f) };
    m_vVertexList[14] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f, 0.0f) };
    m_vVertexList[15] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(1.0f, 0.0f) };
    // 왼쪽- >
    m_vVertexList[16] =PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(0.0f, 1.0f) };
    m_vVertexList[17] =PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f, 0.0f) };
    m_vVertexList[18] =PNCT_VERTEX{ TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(1.0f, 0.0f) };
    m_vVertexList[19] =PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f)  ,TVector2(1.0f, 1.0f) };
    // 오른->면																
    m_vVertexList[20] = PNCT_VERTEX{ TVector3(+w2, h3, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),  TVector2(0.0f, 1.0f) };
    m_vVertexList[21] = PNCT_VERTEX{ TVector3(+w2, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(0.0f, 0.0f) };
    m_vVertexList[22] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(1.0f, 0.0f) };
    m_vVertexList[23] = PNCT_VERTEX{ TVector3(+w2, h3, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f), TVector2(1.0f, 1.0f) };
    m_vIndexList.resize(36);
    
    // 앞면
    m_vIndexList[0] = 0; m_vIndexList[1] = 1; m_vIndexList[2] = 2;
    m_vIndexList[3] = 0; m_vIndexList[4] = 2; m_vIndexList[5] = 3;
    // 뒷면
    m_vIndexList[6] = 4; m_vIndexList[7] = 5; m_vIndexList[8] = 6;
    m_vIndexList[9] = 4; m_vIndexList[10] = 6; m_vIndexList[11] = 7;
    // 윗면
    m_vIndexList[12] = 8; m_vIndexList[13] = 9; m_vIndexList[14] = 10;
    m_vIndexList[15] = 8; m_vIndexList[16] = 10;m_vIndexList[17] = 11;
    // 아랫면
    m_vIndexList[18] = 12; m_vIndexList[19] = 13; m_vIndexList[20] = 14;
    m_vIndexList[21] = 12; m_vIndexList[22] = 14; m_vIndexList[23] = 15;
    // 왼쪽면
    m_vIndexList[24] = 16; m_vIndexList[25] = 17; m_vIndexList[26] = 18;
    m_vIndexList[27] = 16; m_vIndexList[28] = 18; m_vIndexList[29] = 19;
    // 오른쪽면
    m_vIndexList[30] = 20; m_vIndexList[31] = 21; m_vIndexList[32] = 22;
    m_vIndexList[33] = 20; m_vIndexList[34] = 22; m_vIndexList[35] = 23;
}

HRESULT SkyBox::CreateTextureArray()
{
    HRESULT hr = S_OK;
    const wstring sz_textures[] =
    {
        L"../../Resource/Sky/st00_cm_front.bmp",
        L"../../Resource/Sky/st00_cm_back.bmp",
        L"../../Resource/Sky/st00_cm_up.bmp",
        L"../../Resource/Sky/st00_cm_down.bmp",
        L"../../Resource/Sky/st00_cm_left.bmp",
        L"../../Resource/Sky/st00_cm_right.bmp"
       
    };
    int iNumTexture = sizeof(sz_textures) / sizeof(sz_textures[0]);
    for (int iTex = 0; iTex < iNumTexture; ++iTex)
    {
        CTexture* tex0 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(sz_textures[iTex]));
        m_pSkyTexRV[iTex].Attach(tex0->GetSRV());
    }
  
   

   
   //f (FAILED(hr = LoadTextureArray(sz_textures, iNumTexture,
   //   m_pSkyTexture.GetAddressOf(),
   //   m_pSkyTexRV.GetAddressOf())))
   //
   //   return hr;
   //

    return hr;

}

HRESULT SkyBox::LoadTextureArray(const TCHAR** szTextureName, int iNumTex, ID3D11Texture2D** ppTex2D, ID3D11ShaderResourceView** ppSRV)
{

    HRESULT hr = S_OK;

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc,sizeof(desc));

    wstring str[MAX_PATH];

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Height = 512;
    texDesc.Width = 512;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;



    for (int i = 0; i < iNumTex; ++i)
    {
        size_t maxsize = 0;
        auto ImageObj = std::make_unique<DirectX::ScratchImage>();
        DirectX::TexMetadata mData;

        hr = DirectX::GetMetadataFromWICFile(szTextureName[i], DirectX::WIC_FLAGS_NONE, mData);
        if (SUCCEEDED(hr))
        {
            hr = DirectX::LoadFromWICFile(szTextureName[i], DirectX::WIC_FLAGS_NONE, &mData, *ImageObj);
            if (SUCCEEDED(hr))
            {

            }
        }
       // ID3D11Texture2D* texture;
        ID3D11ShaderResourceView* tempSRV;
        ComPtr<ID3D11Resource>		 res;
        hr = DirectX::CreateShaderResourceView(CoreInterface::g_pDevice.Get(), ImageObj->GetImages(),
            ImageObj->GetImageCount(),
            mData, &tempSRV);
        tempSRV->GetResource(res.GetAddressOf());

        if (SUCCEEDED(hr))
        {
            ID3D11Texture2D* pTemp;
            res->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&pTemp);
            pTemp->GetDesc(&desc);

            if (!(*ppTex2D))
            {
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                desc.CPUAccessFlags = 0;
                desc.ArraySize = iNumTex;
                CoreInterface::g_pDevice->CreateTexture2D(&desc, NULL, ppTex2D);
            }
            for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
            {
                D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
                if (SUCCEEDED(CoreInterface::g_pImmediateContext->Map((ID3D11Resource*)pTemp, 0, D3D11_MAP_READ, 0, &MappedFaceDest)))
                {
                    UCHAR* pTexels = (UCHAR*)MappedFaceDest.pData;
                    CoreInterface::g_pImmediateContext->UpdateSubresource((*ppTex2D),
                        D3D10CalcSubresource(iMip, i, desc.MipLevels),
                        NULL,
                        MappedFaceDest.pData,
                        MappedFaceDest.RowPitch,
                        0);
                    CoreInterface::g_pImmediateContext->Unmap(pTemp, 0);
                }
            }

            //texture->Release();
            pTemp->Release();
            //tempSRV->Release();
        }
        else
        {
            return hr;
        }
       
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory(&SRVDesc, sizeof(SRVDesc));
    // 원본 컬러를 사용( rgb )
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // SRGB 포맷으로 변환하면 색상이 강해진다.
    //SRVDesc.Format = MAKE_SRGB(DXGI_FORMAT_R8G8B8A8_UNORM);
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
    SRVDesc.Texture2DArray.ArraySize = iNumTex;
    hr = CoreInterface::g_pDevice->CreateShaderResourceView(*ppTex2D, &SRVDesc, ppSRV);



    return hr;
}

void SkyBox::CreateSamplerState()
{
    D3D11_SAMPLER_DESC descSamp;
    descSamp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    descSamp.MipLODBias = 0;
    descSamp.MaxAnisotropy = 16;

    descSamp.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    descSamp.ComparisonFunc = D3D11_COMPARISON_NEVER;

    descSamp.BorderColor[0] = 1.0f;
    descSamp.BorderColor[1] = 0.0f;
    descSamp.BorderColor[2] = 0.0f;
    descSamp.BorderColor[3] = 1.0f;
    descSamp.MinLOD = 0;
    descSamp.MaxLOD = D3D11_FLOAT32_MAX;
    CoreInterface::g_pDevice->CreateSamplerState(&descSamp, m_pSkyBoxSamplerState.GetAddressOf());



}