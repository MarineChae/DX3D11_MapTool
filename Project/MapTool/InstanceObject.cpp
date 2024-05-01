#include "InstanceObject.h"

bool InstanceObject::Init()
{
	Create(L"", L"../../Resource/Plane.hlsl");
	return true;
}

bool InstanceObject::Frame()
{
	int iObj = 0;
	for (auto& obj : m_InstanceList.first)
	{
		for (auto& child : obj->m_vChildList)
		{
			m_InstanceList.second[iObj].matWorld = child->m_WolrdMatrix;
		}
		
		++iObj;
	}



	
	return true;
}

bool InstanceObject::Render(TMatrix* world , TMatrix* view, TMatrix* proj)
{
	int iObj = 0;
	if (m_InstanceList.first.empty()) return false;
	for (auto& obj : m_InstanceList.first[0]->m_vChildList)
	{
	
		obj->SetMatrix(world, view, proj);
		obj->PreRender();
		for (int isize = 0; isize < m_InstanceList.second.size();  )
		{
			for (auto& obj : m_InstanceList.first)
			{
				//인스턴싱 데이터를 업데이트 해준다
				m_InstanceList.second[isize].matWorld = obj->m_vChildList[iObj]->m_WolrdMatrix;
				isize++;
				
			}
		}
	
		//vs에 인스턴싱 데이터를 추가적으로 넘겨준다
		ID3D11Buffer* Buffer[3] = { obj->m_pVertexBuffer.Get(),obj->m_pTangentVB.Get(), m_InstanceBuffer.Get()};
		UINT stride[3] = { sizeof(PNCT_VERTEX), sizeof(T_VERTEX),sizeof(INSTANCEDATA) };
		UINT offset[3] = { 0, 0 ,0};

		CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 3, Buffer, stride, offset);
		if (m_InstanceBuffer)
		{
			INSTANCEDATA* pInstances = NULL;
			D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
			//인스턴싱 버퍼를 업데이트 해준다.
			if (SUCCEEDED(CoreInterface::g_pImmediateContext->Map((ID3D11Resource*)m_InstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedFaceDest)))
			{
				pInstances = (INSTANCEDATA*)MappedFaceDest.pData;
				memcpy(pInstances, &m_InstanceList.second.at(0), sizeof(INSTANCEDATA) * m_InstanceList.second.size());
				CoreInterface::g_pImmediateContext->Unmap(m_InstanceBuffer.Get(), 0);
			}
		}
		for (int iSubmtrl = 0; iSubmtrl < obj->m_pMesh->m_TriangleList.size(); ++iSubmtrl)
		{
			if (obj->m_vTexArr.size() && obj->m_vTexArr[iSubmtrl])
			{
				obj->m_vTexArr[iSubmtrl]->Apply(CoreInterface::g_pImmediateContext.Get(), 0);
			}
			if (obj->m_vNormalMapArr.size() && obj->m_vNormalMapArr[iSubmtrl])
			{
				obj->m_vNormalMapArr[iSubmtrl]->Apply(CoreInterface::g_pImmediateContext.Get(), 1);
			}
			if (obj->m_vObjectIndexList[iSubmtrl].size() < 3)
				continue;

			CoreInterface::g_pImmediateContext->IASetIndexBuffer(obj->m_vObjectIndexBuffer[iSubmtrl].Get(), DXGI_FORMAT_R32_UINT, 0);
			//DrawInstance 함수를사용해 렌더해야만 인스턴싱이 적용된다.
			CoreInterface::g_pImmediateContext->DrawIndexedInstanced(obj->m_vObjectIndexList[iSubmtrl].size(), m_InstanceList.second.size(), 0, 0,0);

		}
		++iObj;
	}
	
	return false;
}

bool InstanceObject::CreateInstanceBuffer(TMatrix mat)
{

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(INSTANCEDATA) * m_InstanceList.second.size()*500;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	CoreInterface::g_pDevice->CreateBuffer(&vbd, 0, m_InstanceBuffer.GetAddressOf());


    return true;
}
