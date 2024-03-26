#include "ProJectionShadow.h"
#include "CoreInterface.h"
#include "MapObject.h"
#include "RenderTarget.h"
#include "InstanceObject.h"

bool ProJectionShadow::Init()
{
	m_vLightPos = TVector3(15, 400, -35);

	CShaderMgr::GetInstance().Load(L"../../Resource/ShadowObject.hlsl");
	m_pPixelShader.Attach(CShaderMgr::GetInstance().GetPtr(L"ShadowObject.hlsl")->m_pPixelShader.Get());
	m_pVertexShader.Attach(CShaderMgr::GetInstance().GetPtr(L"ShadowObject.hlsl")->m_VertexShader.Get());

	
	CreateSamplerState();

	m_matTexture = TMatrix(0.5f, 0.0f, 0.0f, 0.0f
		, 0.0f, -0.5f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.5f, 0.5f, 0.0f, 1.0f);
	return true;
}
bool ProJectionShadow::Frame()
{
	TMatrix mLightWorld, mTranslate, mRotation;
	D3DXMatrixTranslation(&mTranslate, 100.0f, 100.0f, 0.0f);
	D3DXMatrixRotationY(&mRotation, g_fGameTime);
	D3DXMatrixMultiply(&mLightWorld, &mTranslate, &mRotation);

	m_vLightVector.x = mLightWorld._41;
	m_vLightVector.y = mLightWorld._42;
	m_vLightVector.z = mLightWorld._43;

	D3DXVec3Normalize(&m_vLightVector, &m_vLightVector);
	m_vLightVector *= -1.0f;
	return true;
}
bool ProJectionShadow::Render(RenderTarget rt, map<UINT, shared_ptr<InstanceObject>> list,shared_ptr<CQuadTree> tree)
{	
	TVector4 vClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	if (rt.Begin(vClearColor))
	{
		TVector3 vEye = m_vLightVector;
		TVector3 vLookat = TVector3(0, 0, 0);
		TVector3 vUp = TVector3(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&m_matViewLight, &vEye, &vLookat, &vUp);
		//D3DXMatrixOrthoLH( &m_matProjLight[1], 64/2, 64/2, 0.0f, 100.0f );
		D3DXMatrixOrthoOffCenterLH(&m_matProjLight, -512 / 2, 512 / 2, -512 / 2, 512 / 2, 0.0f, 100.0f);



		for (auto& inst : list)
		{
			for (auto& obj : inst.second->m_InstanceList.first)
			{
				for (auto& child : obj->m_vChildList)
					RenderShadow(&child->m_WolrdMatrix, &m_matShadow, &m_matViewLight, &m_matProjLight, child.get());
			}

		}



		//tree->m_pMap->SetMatrix(&m_matShadow, &m_matViewLight, &m_matProjLight);
		//tree->m_pMap->PreRender();
		//UINT Stride = sizeof(TVector3);
		//UINT offset = 0;
		//CoreInterface::g_pImmediateContext->IASetVertexBuffers(1, 1, tree->m_pMap->m_pTangentVB.GetAddressOf(), &Stride, &offset);//p 159참조
		//CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, tree->m_pTexSRV[0].GetAddressOf());
		//CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 4, tree->m_pTexSRV[1].GetAddressOf());
		//CoreInterface::g_pImmediateContext->PSSetShaderResources(6, 4, tree->m_pNormalMapSRV[0].GetAddressOf());
		//for (auto node : tree->m_vLeafNodeList)
		//{
		//	UINT Stride = sizeof(PNCT_VERTEX);
		//	UINT offset = 0;
		//
		//	CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 1, node->m_pVertexBuffer.GetAddressOf(), &Stride, &offset);//p 159참조
		//	CoreInterface::g_pImmediateContext->IASetIndexBuffer(node->m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		//	CoreInterface::g_pImmediateContext->DrawIndexed(node->m_vIndexList.size(), 0, 0);
		//
		//
		//}


	

		rt.End();
	}
	m_cbShadow.g_matShadow[0] = m_matViewLight * m_matProjLight * m_matTexture;
	D3DXMatrixTranspose(&m_cbShadow.g_matShadow[0], &m_cbShadow.g_matShadow[0]);
	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pShadowCB.Get(), 0, NULL, &m_cbShadow, 0, 0);
	CoreInterface::g_pImmediateContext->VSSetConstantBuffers(1, 1, m_pShadowCB.GetAddressOf());
	return true;
}
void ProJectionShadow::CreateSamplerState()
{
	D3D11_SAMPLER_DESC descSamp;
	descSamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
	CoreInterface::g_pDevice->CreateSamplerState(&descSamp, m_pShadowSS.GetAddressOf());

}
void ProJectionShadow::RenderShadow(TMatrix* matWorld, TMatrix* matShadow,
	TMatrix* matView, TMatrix* matProj, MapObject* obj)
{
	//CoreInterface::g_pImmediateContext->OMSetDepthStencilState(m_pDSS.Get(), 0);

	TMatrix matWorldShadow = (*matWorld) * (*matShadow);
	obj->SetMatrix(&matWorldShadow, matView, matProj);
	obj->PreRender();
	CoreInterface::g_pImmediateContext->VSSetShader(m_pVertexShader.Get(), NULL, 0);
	CoreInterface::g_pImmediateContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);
	ComPtr<ID3D11Buffer> Buffer[2] = { obj->m_pVertexBuffer,obj->m_pTangentVB };
	UINT Stride[2] = { sizeof(PNCT_VERTEX),sizeof(T_VERTEX) };
	UINT offset[2] = { 0,0 };
	CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 2, Buffer->GetAddressOf(), Stride, offset);//p 159참조

	for (int iSubmtrl = 0; iSubmtrl < obj->m_pMesh->m_TriangleList.size(); ++iSubmtrl)
	{
		CoreInterface::g_pImmediateContext->IASetIndexBuffer(obj->m_vObjectIndexBuffer[iSubmtrl].Get(), DXGI_FORMAT_R32_UINT, 0);
		CoreInterface::g_pImmediateContext->DrawIndexed(obj->m_vObjectIndexList[iSubmtrl].size(), 0, 0);
	}

}
