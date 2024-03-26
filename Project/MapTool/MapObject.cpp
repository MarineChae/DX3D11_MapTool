#include "MapObject.h"
bool MapObject::Frame()
{
	m_fCurrentAnimTime += m_AnimInfo.m_iFrameSpeed * g_fSecondPerFrame;
	if (m_fCurrentAnimTime >= m_AnimInfo.m_iEndFrame)
	{
		m_fCurrentAnimTime = m_AnimInfo.m_iStartFrame;
	}
	for (int iSub = 0; iSub < m_vChildList.size(); iSub++)
	{
		auto& obj = m_vChildList[iSub];
		if (!obj->m_pMesh->m_vMatrixArr.empty())
			obj->m_WolrdMatrix = obj->m_pMesh->m_vMatrixArr[(int)m_fCurrentAnimTime];
		else
			obj->m_WolrdMatrix = TMatrix::Identity;

		m_SRTMat = m_Scale;

		TMatrix RotateX;
		TMatrix RotateY;
		TMatrix RotateZ;
		TMatrix Rotate;
		D3DXMatrixRotationX(&RotateX, DegreeTORadian(m_Rotation.x));
		D3DXMatrixRotationY(&RotateY, DegreeTORadian(m_Rotation.y));
		D3DXMatrixRotationZ(&RotateZ, DegreeTORadian(m_Rotation.z));
		Rotate = RotateX * RotateY * RotateZ;

		m_SRTMat *= Rotate;
		m_SRTMat *= m_Transform;
	
		obj->m_WolrdMatrix *= m_SRTMat;
		if (m_bIsUpdate)
		{

			UpdateCollisionBox(obj, m_SRTMat);

			m_bIsUpdate = false;
		}
	}

	return true;
}
bool MapObject::Render(TMatrix mat)
{

	for (int i = 0; i < m_vChildList.size(); ++i)
	{
		auto& obj = m_vChildList[i];

		obj->SetMatrix(nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
		obj->PreRender();

		ComPtr<ID3D11Buffer> Buffer[2] = { obj->m_pVertexBuffer,obj->m_pTangentVB };
		UINT Stride[2] = { sizeof(PNCT_VERTEX),sizeof(T_VERTEX)};
		UINT offset[2] = { 0,0 };
		CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 2, Buffer->GetAddressOf(), Stride, offset);//p 159참조


		obj->PostRender();
	}
	return true;
}

bool MapObject::Release()
{
	CObject::Release();
	for (auto& mesh : m_pFbxObj->m_MeshList)
	{
		mesh->Release();
	}
	for (auto& tex : m_vTexArr)
	{
		delete tex;
	}
	m_vTexArr.clear();
	for (auto& tex : m_vNormalMapArr)
	{
		delete tex;
	}
	m_vChildList.clear();
	m_vObjectIndexList.clear();
	m_vIndexListForTangent.clear();
	m_vObjectIndexBuffer.clear();
	m_vTangentList.clear();
	if(m_pFbxObj!=nullptr)
	{
		m_pFbxObj->Release();
		delete m_pFbxObj;

	}
	return false;
}

void MapObject::LoadTextureArray(std::wstring filename)
{
	auto Texture = CTextureMgr::GetInstance().Load(filename);
	if (Texture == nullptr)
	{
		Texture = CTextureMgr::GetInstance().Load(L"../../Resource/Default.png");
	}
	m_vTexArr.push_back(Texture);
	
}
void MapObject::LoadNormalTextureArray(std::wstring filename)
{
	m_vNormalMapArr.push_back(CTextureMgr::GetInstance().Load(filename));
}
bool MapObject::PostRender()
{

	for (int iSubmtrl = 0; iSubmtrl < m_pMesh->m_TriangleList.size(); ++iSubmtrl)
	{
		if (m_vTexArr.size() && m_vTexArr[iSubmtrl])
		{
			m_vTexArr[iSubmtrl]->Apply(CoreInterface::g_pImmediateContext.Get(), 0);
		}
		if (m_vNormalMapArr.size() && m_vNormalMapArr[iSubmtrl])
		{
			m_vNormalMapArr[iSubmtrl]->Apply(CoreInterface::g_pImmediateContext.Get(), 1);
		}

		//CoreInterface::g_pImmediateContext->Draw(m_pMesh->m_TriangleList[iSubmtrl].size(),
		//	m_pMesh->m_TriangleOffSetList[iSubmtrl]);
		CoreInterface::g_pImmediateContext->IASetIndexBuffer(m_vObjectIndexBuffer[iSubmtrl].Get(), DXGI_FORMAT_R32_UINT, 0);
		CoreInterface::g_pImmediateContext->DrawIndexed(m_vObjectIndexList[iSubmtrl].size(), 0, 0);
	}


	return true;
}

void MapObject::SetMapObj(CFbxObj* obj, AnimDesc& AnimInfo)
{
	m_pFbxObj = obj;
	m_AnimInfo = AnimInfo;
	
	for (int iSubmtrl = 0; iSubmtrl < m_pFbxObj->m_MeshList.size(); ++iSubmtrl)
	{
		auto& fbxmesh = m_pFbxObj->m_MeshList[iSubmtrl];
		auto  Newobj = std::make_shared<MapObject>();

		m_vChildList.push_back(Newobj);
		Newobj->m_vVertexList.resize(fbxmesh->iNumPolygon * 3);
		Newobj->m_pMesh = fbxmesh;

		UINT iNumSubMaterialCount = fbxmesh->m_TriangleList.size();
		UINT iSubVertexIndex = 0;//다음 mtrl 시작 인덱스 
		//VB 하나로 모두 렌더링 하기위해 하는 작업 .
		Newobj->m_vObjectIndexList.resize(iNumSubMaterialCount);
		
		for (int imtrl = 0; imtrl < iNumSubMaterialCount; ++imtrl)
		{
			fbxmesh->m_TriangleOffSetList.push_back(iSubVertexIndex);
			for (int i = 0; i < fbxmesh->m_TriangleList[imtrl].size(); ++i)
			{
				Newobj->m_vVertexList[iSubVertexIndex + i] = fbxmesh->m_TriangleList[imtrl][i];
				Newobj->m_vObjectIndexList[imtrl].push_back(iSubVertexIndex + i);
				Newobj->m_vIndexListForTangent.push_back(iSubVertexIndex + i);
				CheckVertexMinMax(Newobj->m_vVertexList[iSubVertexIndex + i].Pos, Newobj->m_OriginMinSize, Newobj->m_OriginMaxSize);
			}
			iSubVertexIndex += fbxmesh->m_TriangleList[imtrl].size();
			if (!Newobj->m_vObjectIndexList[imtrl].empty())
				CreateObectIndexBuffer(Newobj, imtrl);
			else
			{
				Newobj->m_vObjectIndexList[imtrl].push_back(-1);
				CreateObectIndexBuffer(Newobj, imtrl);
			}
			
		}
		CreateTangentVector(Newobj);

		std::wstring path = L"../../Resource/";
		std::wstring filename;

		if (fbxmesh->m_szTextureFilename.size() >= 1)
		{
			filename = fbxmesh->m_szTextureFilename[0];
			path += filename;
			Newobj->Create(path, L"../../Resource/FBXObject.hlsl");

		}
		else
		{
			Newobj->Create(L"../../Resource/Default.png", L"../../Resource/FBXObject.hlsl");
		}

		Newobj->m_WolrdMatrix = fbxmesh->m_WolrdMat;

 		for (int iSub = 0; iSub < fbxmesh->m_szTextureFilename.size(); iSub++)
		{
			std::wstring filename = fbxmesh->m_szTextureFilename[iSub];
			std::wstring path = L"../../Resource/";
			path += filename;
			Newobj->LoadTextureArray(path);
			//Newobj->LoadNormalTextureArray(L"../../Resource/stone_wall_normal_map.bmp");
		}
		CreateCollisionBox(Newobj, Newobj->m_OriginMinSize, Newobj->m_OriginMaxSize);
	}


	

}
void MapObject::CreateObectIndexBuffer(shared_ptr<MapObject> obj, int num)
{
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = sizeof(DWORD) * obj->m_vObjectIndexList[num].size();//버퍼의 크기
	Desc.Usage = D3D11_USAGE_DEFAULT;//버퍼를 어디에서 읽을것인지
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//버퍼의 사용용도
	Desc.CPUAccessFlags = 0;//cpu의 엑세스유형 필요없으면 null
	Desc.MiscFlags = 0;//추가적인 옵션
	Desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA pInitialData;
	pInitialData.pSysMem = &obj->m_vObjectIndexList[num].at(0);
	ComPtr<ID3D11Buffer> objbuffer;
	CoreInterface::g_pDevice->CreateBuffer(&Desc, &pInitialData, objbuffer.GetAddressOf());

	obj->m_vObjectIndexBuffer.push_back(objbuffer);
}
void MapObject::CreateTangentSpaceVectors(TVector3* v0,
	TVector3* v1, TVector3* v2, TVector2 uv0,
	TVector2 uv1, TVector2 uv2,
	TVector3* vTangent, TVector3* vBiNormal, TVector3* vNormal)
{
	TVector3 vEdge1 = *v1 - *v0;
	TVector3 vEdge2 = *v2 - *v0;
	D3DXVec3Normalize(&vEdge1, &vEdge1);
	D3DXVec3Normalize(&vEdge2, &vEdge2);
	// UV delta
	TVector2 deltaUV1 = uv1 - uv0;
	TVector2 deltaUV2 = uv2 - uv0;
	D3DXVec2Normalize(&deltaUV1, &deltaUV1);
	D3DXVec2Normalize(&deltaUV2, &deltaUV2);


	TVector3 biNormal;
	float fDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	if (fabsf(fDet) < 1e-6f)
	{
		*vTangent = TVector3(1.0f, 0.0f, 0.0f);
		biNormal = TVector3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		*vTangent = (vEdge1 * deltaUV2.y - vEdge2 * deltaUV1.y) * fDet;
		biNormal = (vEdge2 * deltaUV1.x - vEdge1 * deltaUV2.x) * fDet;
	}
	D3DXVec3Normalize(vTangent, vTangent);
	D3DXVec3Normalize(&biNormal, &biNormal);

	D3DXVec3Cross(vBiNormal, vNormal, vTangent);
	float crossinv = (D3DXVec3Dot(vBiNormal, &biNormal) < 0.0f) ? -1.0f : 1.0f;
	*vBiNormal *= crossinv;
}


void MapObject::CreateTangentVector(shared_ptr<MapObject> obj)
{
	TVector3 vTangent, vBiNormal, vNormal;
	int iIndex = 0;
	int i0, i1, i2;

	obj->m_vTangentList.resize(obj->m_vVertexList.size());
	for (int iIndex = 0; iIndex < obj->m_vIndexListForTangent.size(); iIndex += 3)
	{
		i0 = obj->m_vIndexListForTangent[iIndex + 0];
		i1 = obj->m_vIndexListForTangent[iIndex + 1];
		i2 = obj->m_vIndexListForTangent[iIndex + 2];

		CreateTangentSpaceVectors(&obj->m_vVertexList[i0].Pos, &obj->m_vVertexList[i1].Pos, &obj->m_vVertexList[i2].Pos,
			obj->m_vVertexList[i0].Tex, obj->m_vVertexList[i1].Tex, obj->m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &obj->m_vVertexList[i0].Nor);

		obj->m_vTangentList[i0].Tangent += vTangent;

		i0 = obj->m_vIndexListForTangent[iIndex + 1];
		i1 = obj->m_vIndexListForTangent[iIndex + 2];
		i2 = obj->m_vIndexListForTangent[iIndex + 0];

		CreateTangentSpaceVectors(&obj->m_vVertexList[i0].Pos, &obj->m_vVertexList[i1].Pos, &obj->m_vVertexList[i2].Pos,
			obj->m_vVertexList[i0].Tex, obj->m_vVertexList[i1].Tex, obj->m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &obj->m_vVertexList[i0].Nor);

		obj->m_vTangentList[i0].Tangent += vTangent;

		i0 = obj->m_vIndexListForTangent[iIndex + 2];
		i1 = obj->m_vIndexListForTangent[iIndex + 0];
		i2 = obj->m_vIndexListForTangent[iIndex + 1];

		CreateTangentSpaceVectors(&obj->m_vVertexList[i0].Pos, &obj->m_vVertexList[i1].Pos, &obj->m_vVertexList[i2].Pos,
			obj->m_vVertexList[i0].Tex, obj->m_vVertexList[i1].Tex, obj->m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &obj->m_vVertexList[i0].Nor);

		obj->m_vTangentList[i0].Tangent += vTangent;

	}

	for (int i = 0; i < obj->m_vVertexList.size(); i++)
	{
		D3DXVec3Normalize(&obj->m_vTangentList[i].Tangent, &obj->m_vTangentList[i].Tangent);
	}
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = sizeof(T_VERTEX) * obj->m_vTangentList.size();
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = &obj->m_vTangentList.at(0);
	CoreInterface::g_pDevice->CreateBuffer(
		&Desc,
		&InitialData,
		obj->m_pTangentVB.GetAddressOf());



}


void MapObject::CreateCollisionBox(shared_ptr<MapObject> obj,TVector3 Min, TVector3 Max)
{

	obj->m_CollisionBox.m_Max.x = Max.x;
	obj->m_CollisionBox.m_Max.z = Max.z;
	obj->m_CollisionBox.m_Min.x = Min.x;
	obj->m_CollisionBox.m_Min.z = Min.z;

	obj->m_CollisionBox.m_Max.y = Max.x;
	obj->m_CollisionBox.m_Min.y = Min.y;

	obj->m_CollisionBox.m_vAxis[0] = { 1,0,0 };
	obj->m_CollisionBox.m_vAxis[1] = { 0,1,0 };
	obj->m_CollisionBox.m_vAxis[2] = { 0,0,1 };

	obj->m_CollisionBox.m_pCenter =   (obj->m_CollisionBox.m_Max   + obj->m_CollisionBox.m_Min) * 0.5f;
	obj->m_CollisionBox.m_fExtent[0] = obj->m_CollisionBox.m_Max.x - obj->m_CollisionBox.m_pCenter.x;
	obj->m_CollisionBox.m_fExtent[1] = obj->m_CollisionBox.m_Max.y - obj->m_CollisionBox.m_pCenter.y;
	obj->m_CollisionBox.m_fExtent[2] = obj->m_CollisionBox.m_Max.z - obj->m_CollisionBox.m_pCenter.z;
	
	

}
void MapObject::UpdateCollisionBox(shared_ptr<MapObject> obj, TMatrix SRT)
{
	D3DXVec3TransformCoord(&obj->m_CollisionBox.m_Max, &obj->m_OriginMaxSize,&SRT);
	D3DXVec3TransformCoord(&obj->m_CollisionBox.m_Min, &obj->m_OriginMinSize, &SRT);

	obj->m_CollisionBox.m_pCenter = (obj->m_CollisionBox.m_Max + obj->m_CollisionBox.m_Min) * 0.5f;
	obj->m_CollisionBox.m_fExtent[0] = obj->m_CollisionBox.m_Max.x - obj->m_CollisionBox.m_pCenter.x;
	obj->m_CollisionBox.m_fExtent[1] = obj->m_CollisionBox.m_Max.y - obj->m_CollisionBox.m_pCenter.y;
	obj->m_CollisionBox.m_fExtent[2] = obj->m_CollisionBox.m_Max.z - obj->m_CollisionBox.m_pCenter.z;
	obj->DebugBox();
}
void MapObject::CheckVertexMinMax(const TVector3 pos, TVector3& Min, TVector3& Max)
{
	if (pos.x > Max.x)
	{
		Max.x = pos.x;
	}
	if (pos.y > Max.y)
	{
		Max.y = pos.y;
	}
	if (pos.z > Max.z)
	{
		Max.z = pos.z;
	}

	if (pos.x < Min.x)
	{
		Min.x = pos.x;
	}
	if (pos.y < Min.y)
	{
		Min.y = pos.y;
	}
	if (pos.z < Min.z)
	{
		Min.z = pos.z;
	}

}

bool MapObject::CreateInputLayout()
{

	D3D11_INPUT_ELEMENT_DESC InputElementDescs[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},

		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		
		{"INST",0,DXGI_FORMAT_R32G32B32A32_FLOAT,2,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INST",1,DXGI_FORMAT_R32G32B32A32_FLOAT,2,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INST",2,DXGI_FORMAT_R32G32B32A32_FLOAT,2,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"INST",3,DXGI_FORMAT_R32G32B32A32_FLOAT,2,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1},
		{"EMISSIVE",0,DXGI_FORMAT_R32_FLOAT,2,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1}

	};

	HRESULT hr = CoreInterface::g_pDevice->CreateInputLayout(
		InputElementDescs,
		sizeof(InputElementDescs) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		m_pShader->m_vBb->GetBufferPointer(),
		m_pShader->m_vBb->GetBufferSize(),
		m_pInputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		LPCSTR errorText = "nullptr";
		errorText = (LPCSTR)hr;

		MessageBoxA(NULL, errorText, "Error", MB_OK | MB_ICONERROR);
	}
	return true;
}

void MapObject::DebugBox()
{
	TestBox = make_shared<CObject>();
	TestBox->m_vVertexList.resize(24);
	float w2 = m_CollisionBox.m_Max.x;
	float h2 = m_CollisionBox.m_Max.y;
	float d2 = m_CollisionBox.m_Max.z;

	float w3 = m_CollisionBox.m_Min.x;
	float h3 = m_CollisionBox.m_Min.y;
	float d3 = m_CollisionBox.m_Min.z;

	// 앞면
	TestBox->m_vVertexList[0] = PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[1] = PNCT_VERTEX{ TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[2] = PNCT_VERTEX{ TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[3] = PNCT_VERTEX{ TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	// 뒷면 ->                                                         TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[4] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[5] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[6] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[7] = PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	// 윗면 ->
	TestBox->m_vVertexList[8] = PNCT_VERTEX{ TVector3(w3, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[9] = PNCT_VERTEX{ TVector3(w3, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[10] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[11] = PNCT_VERTEX{ TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	// 아랫- >
	TestBox->m_vVertexList[12] = PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[13] = PNCT_VERTEX{ TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[14] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[15] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	// 왼쪽- >
	TestBox->m_vVertexList[16] =PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[17] =PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[18] =PNCT_VERTEX{ TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[19] =PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	// 오른->면																
	TestBox->m_vVertexList[20] = PNCT_VERTEX{ TVector3(+w2, h3, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[21] = PNCT_VERTEX{ TVector3(+w2, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[22] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vVertexList[23] = PNCT_VERTEX{ TVector3(+w2, h3, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
	TestBox->m_vIndexList.resize(36);

	// 앞면
	TestBox->m_vIndexList[0] = 0; TestBox->m_vIndexList[1] = 1; TestBox->m_vIndexList[2] = 2;
	TestBox->m_vIndexList[3] = 0; TestBox->m_vIndexList[4] = 2; TestBox->m_vIndexList[5] = 3;
	// 뒷면
	TestBox->m_vIndexList[6] = 4; TestBox->m_vIndexList[7] = 5; TestBox->m_vIndexList[8] = 6;
	TestBox->m_vIndexList[9] = 4; TestBox->m_vIndexList[10] = 6; TestBox->m_vIndexList[11] = 7;
	// 윗면
	TestBox->m_vIndexList[12] = 8; TestBox->m_vIndexList[13] = 9; TestBox->m_vIndexList[14] = 10;
	TestBox->m_vIndexList[15] = 8; TestBox->m_vIndexList[16] = 10; TestBox->m_vIndexList[17] = 11;
	// 아랫면
	TestBox->m_vIndexList[18] = 12; TestBox->m_vIndexList[19] = 13; TestBox->m_vIndexList[20] = 14;
	TestBox->m_vIndexList[21] = 12; TestBox->m_vIndexList[22] = 14; TestBox->m_vIndexList[23] = 15;
	// 왼쪽면
	TestBox->m_vIndexList[24] = 16; TestBox->m_vIndexList[25] = 17; TestBox->m_vIndexList[26] = 18;
	TestBox->m_vIndexList[27] = 16; TestBox->m_vIndexList[28] = 18; TestBox->m_vIndexList[29] = 19;
	// 오른쪽면
	TestBox->m_vIndexList[30] = 20; TestBox->m_vIndexList[31] = 21; TestBox->m_vIndexList[32] = 22;
	TestBox->m_vIndexList[33] = 20; TestBox->m_vIndexList[34] = 22; TestBox->m_vIndexList[35] = 23;

	TestBox->Create(L"../../Resource/019.bmp", L"../../Resource/MultiTex.hlsl");
}

