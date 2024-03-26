#include "WaterMap.h"


bool WaterMap::Create(std::wstring FileName, std::wstring ShaderFileName)
{

	CreateConstantBuffer();
	CreateVertexData();
	CreateIndexData();
	CreateIndexBuffer();
	CreateVertexBuffer();
	m_pShader = CShaderMgr::GetInstance().Load(ShaderFileName);
	CreateInputLayout();
	LoadTextures(FileName);
	UpdataMatrix();

	return false;
}

HRESULT WaterMap::LoadTextures(const wstring LoadTextureString)
{
	HRESULT hr = S_OK;

	const CTexture* water = CTextureMgr::GetInstance().Load(LoadTextureString);
	m_pWaterTexRV.Attach(const_cast<CTexture*>(water)->GetSRV());
	const CTexture* normal = CTextureMgr::GetInstance().Load(L"../../Resource/oceanBump.png");
	if (normal != nullptr)
	m_pNormalRV.Attach(const_cast<CTexture*>(normal)->GetSRV());

	return hr;
}

TVector4 WaterMap::GetColorOfVertex(UINT Index)
{
	return TVector4(1.0f,1.0f,1.0f,1.0f);
}

bool WaterMap::CreateRS()
{
	D3D11_RASTERIZER_DESC rsdesc;
	ZeroMemory(&rsdesc, sizeof(rsdesc));
	rsdesc.CullMode = D3D11_CULL_BACK;
	rsdesc.FillMode = D3D11_FILL_SOLID;
	CoreInterface::g_pDevice->CreateRasterizerState(&rsdesc, m_pRasterizerBackCullSolid.GetAddressOf());
	
	rsdesc.CullMode = D3D11_CULL_NONE;
	CoreInterface::g_pDevice->CreateRasterizerState(&rsdesc, m_pRasterizerCullNoneSolid.GetAddressOf());

	
	return true;

}

bool WaterMap::Init()
{
	CMap::Init();
	CreateRS();
	m_WaterEffectRT[0].Create(1024, 1024);
	m_WaterEffectRT[1].Create(1024, 1024);
	m_WaterEffectRT[2].Create(1024, 1024);

	return false;
}

bool WaterMap::PreRender()
{
	CObject::PreRender();
	return true;
}

bool WaterMap::Render(TMatrix* world, TMatrix* view, TMatrix* Proj)
{
	PreRender();
	SetMatrix(world,view,Proj);


	m_ConstantData.MeshColor.w =
		g_fGameTime*0.1f;// g_fSecPerFrame;
	m_pRefractRV = m_WaterEffectRT[1].m_pSRV;
	m_pReflectRV = m_WaterEffectRT[2].m_pSRV;



	PostRender();
	return true;
}

bool WaterMap::PostRender()
{
	CoreInterface::g_pImmediateContext->PSSetShaderResources(0, 1, m_pWaterTexRV.GetAddressOf());
	CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, m_pRefractRV.GetAddressOf());
	CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 1, m_pReflectRV.GetAddressOf());
	CoreInterface::g_pImmediateContext->PSSetShaderResources(3, 1, m_pNormalRV.GetAddressOf());
	CObject::PostRender();

	return true;
}

bool WaterMap::Load(CMapDesc& MapDesc, std::vector<float> heightlist)
{
	if (!CreateMap(MapDesc))
	{
		return false;
	}
	return true;
}

WaterMap::WaterMap()
{
}

WaterMap::~WaterMap()
{
}
