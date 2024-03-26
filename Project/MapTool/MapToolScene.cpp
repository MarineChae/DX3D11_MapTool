#include "MapToolScene.h"
#include"CCollision.h"
#include"ImGuiManager.h"




bool MapToolScene::Init()
{
	CWriter::GetInstance().m_bValid = false;
	m_pSkyBox = make_shared<SkyBox>();
	m_pSkyBox->Init();
	m_pHeightMap = make_shared<CHeightMap>();
	m_pHeightMap->Init();
	m_pDebugCamera = std::make_shared<CDebugCamera>();
	m_pDebugCamera->PerspectiveFovLH(1.0f, 1000.f, PI * 0.25, (float)gWindow_Width / (float)gWindow_Height);
	m_pDebugCamera->ViewLookAtLH({ 0,200,-50 }, { 0,0,1 }, { 0,1,0 });
	CoreInterface::g_pMainCamera = m_pDebugCamera;
	m_pQuadTree = std::make_shared<CQuadTree>();
	m_pQuadTree->BuildQuadTree(m_pHeightMap, m_pHeightMap->m_iRow, m_pHeightMap->m_iCol, m_pHeightMap->m_vVertexList);
	m_RedoUndo.Init(m_pQuadTree);


	m_WaterMap.Init();
	CMapDesc WaterMapDesc = { m_pHeightMap->m_iRow,
		m_pHeightMap->m_iCol,
		1.0f, 0.0f,
		L"../../Resource/water.bmp",
		L"../../Resource/WaterMap.hlsl" };
	m_WaterMap.Load(WaterMapDesc);




	std::shared_ptr<CFbxObj> loadobj = CModelMgr::GetInstance().Load(L"../../Resource/MultiCameras.FBX");
	std::shared_ptr<CFbxObj> loadobj2 = CModelMgr::GetInstance().Load(L"../../Resource/humanoid.FBX");
	std::shared_ptr<CFbxObj> loadobj3 = CModelMgr::GetInstance().Load(L"../../Resource/Turret_Deploy1.FBX");
	std::shared_ptr<CFbxObj> loadobj5 = CModelMgr::GetInstance().Load(L"../../Resource/pm0132_00.FBX");
	std::shared_ptr<CFbxObj> loadobj4 = CModelMgr::GetInstance().Load(L"../../Resource/BlackCow.fbx");
	
	ObjList.push_back(loadobj);
	ObjList.push_back(loadobj2);
	ObjList.push_back(loadobj3);
	ObjList.push_back(loadobj4);
	ObjList.push_back(loadobj5);

	{
		lightDesc.Ambient = TVector4(1, 1, 1, 1);
		lightDesc.Diffuse = TVector4(1, 1, 1, 1);
		lightDesc.Specular = TVector4(1, 1, 1, 1);
		CreateConstantBuffer(lightDesc, m_pLightBuffer.GetAddressOf());
	}
	{
		ChangeEvreyFrameDesc.CameraPos = CoreInterface::g_pMainCamera->m_vCameraPos;
		ChangeEvreyFrameDesc.CamereaDir = TVector3(CoreInterface::g_pMainCamera->m_vLook.x,
			CoreInterface::g_pMainCamera->m_vLook.y,
			CoreInterface::g_pMainCamera->m_vLook.z);
		ChangeEvreyFrameDesc.LightDirection = TVector3(0, -1, 0);
		CreateConstantBuffer(ChangeEvreyFrameDesc, m_pChangeEvreyFrameCB.GetAddressOf());
	}
	{

		m_Fog.g_ClipPlane = TVector4(0, 1.0f, 0, -30.0f);
		m_Fog.g_FogColor = TVector4(1.0f, 1.0f, 1.0f, 1);
		m_Fog.g_FogInfo.x = FOGMODE_LINEAR;
		m_Fog.g_FogInfo.y = 0.05f;
		m_Fog.g_FogInfo.z = 10.0f;
		m_Fog.g_FogInfo.w = 500.0f;
		CreateConstantBuffer(m_Fog, m_pFogCB.GetAddressOf());
		

	}

	m_RT.Create(1024, 1024);
	m_ProjectionShadow.Init();
	CreateConstantBuffer(m_ProjectionShadow.m_cbShadow, m_ProjectionShadow.m_pShadowCB.GetAddressOf());


	return true;
}
bool MapToolScene::Frame()
{
	float w = -m_fWaterHeight + cosf(g_fGameTime) * m_fWaterHeight * 0.2f;
	m_ProjectionShadow.Frame();
	

	ChangeEvreyFrameDesc.CameraPos = TVector3(CoreInterface::g_pMainCamera->m_vCameraPos.x,
		CoreInterface::g_pMainCamera->m_vCameraPos.y,
		CoreInterface::g_pMainCamera->m_vCameraPos.z);
	ChangeEvreyFrameDesc.CamereaDir = TVector3(CoreInterface::g_pMainCamera->m_vLook.x,
		CoreInterface::g_pMainCamera->m_vLook.y,
		CoreInterface::g_pMainCamera->m_vLook.z);
	ChangeEvreyFrameDesc.LightDirection = m_ProjectionShadow.m_vLightVector;

	m_Fog.g_vCameraPos.x = CoreInterface::g_pMainCamera->m_vCameraPos.x;
	m_Fog.g_vCameraPos.y = CoreInterface::g_pMainCamera->m_vCameraPos.y;
	m_Fog.g_vCameraPos.z = CoreInterface::g_pMainCamera->m_vCameraPos.z;
	m_Fog.g_vCameraPos.w = 1.0f;


	SaveLoad(); 
	m_Select.SetMatrix(nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
	m_pQuadTree->Frame(&m_Select);
	m_RedoUndo.Frame(m_pHeightMap, m_InstanceObjList);
	if (m_pQuadTree->m_BrushType == BrushType::OBJECTSELECT)
	{
		if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_PUSH)
		{
			for (auto& instobj : m_InstanceObjList)
			{
				for (int iobj = 0 ; iobj < instobj.second->m_InstanceList.first.size(); ++iobj)
				{
					auto& obj = instobj.second->m_InstanceList.first[iobj];
					for (auto& child : obj->m_vChildList)
					{
						if (m_Select.GetIntersectionBox(&child->m_CollisionBox))
						{

							if (m_SelectInstanceObject != nullptr)
							{
								m_SelectInstanceObject->m_bBeforeSelect = false;
								m_SelectInstanceObject->m_InstanceList.second[m_BeforeKey].Emissive = 0.0f;
								
								m_SelectInstanceObject = instobj.second;
								m_SelectMapObj = obj;
								m_SelectInstanceObject->m_bBeforeSelect = true;
								instobj.second->m_InstanceList.second[iobj].Emissive = 1.0f;
								m_BeforeKey = iobj;
							}
							else
							{
								m_SelectInstanceObject = instobj.second;
								m_SelectMapObj = obj;
								m_SelectInstanceObject->m_bBeforeSelect = true;
								instobj.second->m_InstanceList.second[iobj].Emissive = 1.0f;
								m_BeforeKey = iobj;
							}	
							break;
						}
					}

				}
			}
		}
	}
	if (m_pQuadTree->m_BrushType > BrushType::OBJECTSELECT)
	{
		for (auto& node : m_pQuadTree->DrawNodeList)
		{
			if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_PUSH && m_Select.GetIntersectionBox(&node->m_Box))
			{
				int currindex = static_cast<int>(m_pQuadTree->m_BrushType) - static_cast<int>(BrushType::OBJECT1);
				auto obj = std::make_shared<MapObject>();
				obj->SetMapObj(ObjList[currindex].get(), ObjList[currindex]->m_AnimInfo);
				obj->m_Transform._41 = m_Select.m_vIntersection.x;
				obj->m_Transform._42 = node->m_Box.m_Max.y;
				obj->m_Transform._43 = m_Select.m_vIntersection.z;
				obj->m_MapObjName = obj->m_pFbxObj->m_csName;

				m_pQuadTree->CheckDuplicationName(obj->m_MapObjName);
				m_pQuadTree->m_MapObjList.insert(make_pair(obj->m_MapObjName, obj));

				auto instanceObj = m_InstanceObjList.find((UINT)ObjList[currindex].get());
				if (instanceObj == m_InstanceObjList.end())
				{
					shared_ptr<InstanceObject> instObj = make_shared<InstanceObject>();
					instObj->m_InstanceList.first.push_back(obj);
					
					INSTANCEDATA data;
					data.matWorld = obj->m_SRTMat;
					instObj->m_InstanceList.second.push_back(data);
					instObj->CreateInstanceBuffer(obj->m_SRTMat);
					m_InstanceObjList.insert(make_pair((UINT)ObjList[currindex].get(), instObj));
				}
				else
				{
					
					instanceObj->second->m_szName = obj->m_pFbxObj->m_csName;
					INSTANCEDATA data;
					data.matWorld = obj->m_SRTMat;
					instanceObj->second->m_InstanceList.first.push_back(obj);
					instanceObj->second->m_InstanceList.second.push_back(data);

				}


			}
		}


	}

	int iObj = 0;
	for (auto& obj : m_pQuadTree->m_MapObjList)
	{
		obj.second->Frame();
	}
	for (auto& inst : m_InstanceObjList)
	{
		inst.second->Frame();
	}
	


	CoreInterface::g_pImmediateContext.Get()->RSSetState(m_WaterMap.m_pRasterizerBackCullSolid.Get());

	TVector4 m_ClipPlaneFront = TVector4(0, 1.0f, 0, w);
	m_Fog.g_FogColor = TVector4(0.0f, 0.0f, 0.0f, 1);
	m_Fog.g_ClipPlane = m_ClipPlaneFront;
	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pFogCB.Get(), 0, NULL, &m_Fog, 0, 0);
	DrawReflectMap(0, nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);

	TVector4 m_ClipPlaneBack = TVector4(0, -1.0f, 0, -w);
	m_Fog.g_ClipPlane = m_ClipPlaneBack;
	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pFogCB.Get(), 0, NULL, &m_Fog, 0, 0);
	DrawReflectMap(1, nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
	
	CoreInterface::g_pImmediateContext.Get()->RSSetState(m_WaterMap.m_pRasterizerCullNoneSolid.Get());
	TMatrix matReflectView;
	D3DXMatrixIdentity(&matReflectView);
	TPlane p = TPlane(0.0f, 1.0f, 0.0f, w);
	D3DXMatrixReflect(&matReflectView, &p);
	TMatrix matView = CoreInterface::g_pMainCamera->m_ViewMatrix;
	D3DXMatrixMultiply(&matReflectView, &matReflectView, &matView);
	m_Fog.g_ClipPlane = TVector4(0, 1.0f, 0, w);
	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pFogCB.Get(), 0, NULL, &m_Fog, 0, 0);
	DrawReflectMap(2, nullptr, &matReflectView, &CoreInterface::g_pMainCamera->m_ProjMatrix);





	SelectMenu();
	return true;
}
bool MapToolScene::Render()
{
	


	float w = -m_fWaterHeight + cosf(g_fGameTime) * m_fWaterHeight * 0.2f;
	{
		CoreInterface::g_pImmediateContext->UpdateSubresource(m_pChangeEvreyFrameCB.Get(), 0, NULL, &ChangeEvreyFrameDesc, 0, 0);
		CoreInterface::g_pImmediateContext->VSSetConstantBuffers(1, 1, m_pLightBuffer.GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetConstantBuffers(1, 1, m_pLightBuffer.GetAddressOf());
		CoreInterface::g_pImmediateContext->VSSetConstantBuffers(2, 1, m_pChangeEvreyFrameCB.GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetConstantBuffers(2, 1, m_pChangeEvreyFrameCB.GetAddressOf());
	}

	m_Fog.g_FogColor = TVector4(1.0f, 1.0f, 1.0f, 1);
	m_Fog.g_ClipPlane = TVector4(0, 1.0f, 0, m_fWaterHeight);
	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pFogCB.Get(), 0, NULL, &m_Fog, 0, 0);

	m_ProjectionShadow.Render(m_RT,m_InstanceObjList,m_pQuadTree);


	CoreInterface::g_pImmediateContext->PSSetShaderResources(10, 1, m_RT.m_pSRV.GetAddressOf());
	CoreInterface::g_pImmediateContext->VSSetConstantBuffers(3, 1, m_ProjectionShadow.m_pShadowCB.GetAddressOf());
	
	CoreInterface::g_pImmediateContext->PSSetSamplers(1, 1, m_ProjectionShadow.m_pShadowSS.GetAddressOf());
	m_pQuadTree->m_pMap->SetMatrix(nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
	m_pQuadTree->Render(CoreInterface::g_pMainCamera->m_Plane);
	
	for (auto& inst : m_InstanceObjList)
	{
		inst.second->Render(nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
	
	}



	TMatrix matWaterWorld;
	D3DXMatrixIdentity(&matWaterWorld);
	matWaterWorld._42 = -w;

	m_WaterMap.Render(&matWaterWorld, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);

	
	m_pSkyBox->SetMatrix(nullptr, &CoreInterface::g_pMainCamera->m_ViewMatrix, &CoreInterface::g_pMainCamera->m_ProjMatrix);
	m_pSkyBox->Render();
	
	return true;
}
bool MapToolScene::Release()
{
	for (auto& node : m_pQuadTree->DrawNodeList)
	{
		node->Release();
	}
	m_pQuadTree->Release();

	for (auto& box : TestBoxlist)
	{
		box->Release();
		delete box;
	}
	for (auto& mapobj : m_pQuadTree->m_MapObjList)
	{
		for (int iSub = 0; iSub < mapobj.second->m_vChildList.size(); iSub++)
		{
			auto& obj = mapobj.second->m_vChildList[iSub];
			obj->Release();
		}
		mapobj.second->Release();
	}
	m_pQuadTree->m_MapObjList.clear();

	for (auto& obj : ObjList)
	{
		obj.reset();
	}

	return true;
}
void MapToolScene::DebugCameraData()
{
	{
		std::wstring msg = L"[CameraPos]";
		msg += L"[X]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_WorldMatrix._41);
		msg += L"[Y]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_WorldMatrix._42);
		msg += L"[Z]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_WorldMatrix._43);
		CWriter::GetInstance().AddText(msg, 0, 100, D2D1::ColorF(1, 1, 0, 1));
	}
	{
		std::wstring msg = L"[Look]";
		msg += L"[X]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_vLook.x);
		msg += L"[Y]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_vLook.y);
		msg += L"[Z]";
		msg += std::to_wstring(CoreInterface::g_pMainCamera->m_vLook.z);
		CWriter::GetInstance().AddText(msg, 0, 200, D2D1::ColorF(1, 1, 0, 1));
	}


}
void MapToolScene::DebugPlane()
{

	//CObject* TestBox = new CObject;

	//TestBox->m_vVertexList.resize(24);
	//TestBox->m_vVertexList[0].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[0]);
	//TestBox->m_vVertexList[1].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[1]);
	//TestBox->m_vVertexList[2].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[4]);
	//										
	//TestBox->m_vVertexList[3].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[4]);
	//TestBox->m_vVertexList[4].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[1]);
	//TestBox->m_vVertexList[5].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[5]);
	//										
	//TestBox->m_vVertexList[6].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[5]);
	//TestBox->m_vVertexList[7].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[6]);
	//TestBox->m_vVertexList[8].Pos = TVector3(CoreInterface::g_pMainCamera->m_vFrustumPoint[4]);
	//										
	//TestBox->m_vVertexList[9].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[4]);
	//TestBox->m_vVertexList[10].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[6]);
	//TestBox->m_vVertexList[11].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[7]);
	//										 
	//TestBox->m_vVertexList[12].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[6]);
	//TestBox->m_vVertexList[13].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[2]);
	//TestBox->m_vVertexList[14].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[7]);
	//										 
	//TestBox->m_vVertexList[15].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[7]);
	//TestBox->m_vVertexList[16].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[2]);
	//TestBox->m_vVertexList[17].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[3]);
	//										 
	//TestBox->m_vVertexList[12].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[4]);
	//TestBox->m_vVertexList[13].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[7]);
	//TestBox->m_vVertexList[14].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[0]);
	//										 
	//TestBox->m_vVertexList[15].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[0]);
	//TestBox->m_vVertexList[16].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[7]);
	//TestBox->m_vVertexList[17].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[3]);
	//										 
	//TestBox->m_vVertexList[18].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[5]);
	//TestBox->m_vVertexList[19].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[6]);
	//TestBox->m_vVertexList[20].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[1]);
	//										 
	//TestBox->m_vVertexList[21].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[1]);
	//TestBox->m_vVertexList[22].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[6]);
	//TestBox->m_vVertexList[23].Pos = TVector3(m_pDebugCamera->m_vFrustumPoint[2]);

	//
	////5  6
	////4  7
	//// 
	////1  2
	////0  3
	//TestBox->CreateVertexBuffer();
	//TestBox->Create(L"", L"../../Resource/Plane.hlsl");

	//TestBoxlist.push_back(TestBox);


}
void MapToolScene::SaveLoad()
{
	static bool   m_bImguiLoad = false;
	static bool   m_bImguiSave = false;
	static bool   OpenMenu = false;
	static bool   OpenSave = false;
	static bool   OpenLoad = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Load", NULL))
			{
				OpenLoad = true;

			}
				
			if (ImGui::MenuItem("Save", NULL, OpenSave))
			{
				OpenSave = true;
			
			}
			if (ImGui::MenuItem("Debug", NULL))
			{
				if(CWriter::GetInstance().m_bValid)
					CWriter::GetInstance().m_bValid = false;
				else
					CWriter::GetInstance().m_bValid = true;

			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (OpenLoad)
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".txt", ".");

		// display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", OpenLoad))
		{
			OpenLoad = false;
			m_bImguiLoad = true;
		}
	}
	if (OpenSave)
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".txt", ".");

		// display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", OpenSave))
		{
			OpenSave = false;
			m_bImguiSave = true;
		}

	}

	std::string filePath;
	std::string filePathName;
	if (ImGuiFileDialog::Instance()->IsOk())
	{
		filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
		filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
		ImGuiFileDialog::Instance()->Close();
	}
	



	if (m_bImguiLoad && !filePathName.empty())
	{
		m_bImguiLoad = false;
		m_SaveLoader.LoadMap(filePathName,m_InstanceObjList);
		m_pHeightMap->Release();
		m_pQuadTree->m_pTexSRV[0].Reset();
		m_pQuadTree->Release();
		m_InstanceObjList.clear();
		m_pHeightMap->Init();
		m_pHeightMap->m_bStaticLight = true;
		CMapDesc MapDesc = { m_SaveLoader.m_Mapsize[0],  m_SaveLoader.m_Mapsize[1],1.0f,1.0f,m_pQuadTree->m_pTextureList[1]->m_csName,L"../../Resource/MultiTex.hlsl" };
		m_pHeightMap->Load(MapDesc,m_SaveLoader.m_MapHeight);
		m_pQuadTree->BuildQuadTree(m_pHeightMap, m_pHeightMap->m_iRow, m_pHeightMap->m_iCol, m_pHeightMap->m_vVertexList);
	
		CTexture* tex0 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(m_SaveLoader.m_AlphaTexture));
		CTexture* tex1 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(m_SaveLoader.m_SubTexture[0]));
		m_pQuadTree->m_pTexSRV[1] = tex1->GetSRV();
		CTexture* tex2 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(m_SaveLoader.m_SubTexture[1]));
		m_pQuadTree->m_pTexSRV[2] = (tex2->GetSRV());
		CTexture* tex3 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(m_SaveLoader.m_SubTexture[2]));
		m_pQuadTree->m_pTexSRV[3] = ((tex3->GetSRV()));
		CTexture* tex4 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(m_SaveLoader.m_SubTexture[3]));
		m_pQuadTree->m_pTexSRV[4] = (tex4->GetSRV());
	
		int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
	
	
		ComPtr<ID3D11Resource> res;
	
		tex0->GetSRV()->GetResource(res.GetAddressOf());
		auto scratchImage = make_unique<ScratchImage>();
		DirectX::CaptureTexture(CoreInterface::g_pDevice.Get(), CoreInterface::g_pImmediateContext.Get(), res.Get(), *scratchImage);
	
		BYTE* Texel = reinterpret_cast<BYTE*>(scratchImage->GetPixels());
	
		int row = scratchImage->GetMetadata().height;
		int col = scratchImage->GetMetadata().width;
	
		for (UINT x = 0; x < row; ++x)
		{
			for (UINT y = 0; y < col; ++y)
			{
				BYTE* pixel = &m_pQuadTree->m_pMap->m_fLookup[y * 4 + x * col * 4];
	
				pixel[0] = Texel[y * 4 + x * col * 4 + 2];
				pixel[1] = Texel[y * 4 + x * col * 4 + 1];
				pixel[2] = Texel[y * 4 + x * col * 4 + 0];
				pixel[3] = Texel[y * 4 + x * col * 4 + 3];
			}
		}
		UINT const DataSize = sizeof(BYTE) * 4;
		UINT const RowPitch = DataSize * scratchImage->GetMetadata().width;
		CoreInterface::g_pImmediateContext->UpdateSubresource(m_pQuadTree->m_pMap->m_pRoughnessLookUpTex.Get(), 0, NULL, m_pQuadTree->m_pMap->m_fLookup, RowPitch, 0);
	
		for (auto& mapObject : m_SaveLoader.m_MapObject)
		{
			std::size_t pos = mapObject.first.rfind(L"#");
			std::wstring objName = mapObject.first.substr(0, pos);
	
			for (auto& list : ObjList)
			{
				if (list->m_csName == objName)
				{	
					auto obj = m_pQuadTree->SetUpMapObject(list,
						TVector3(mapObject.second._11, mapObject.second._12, mapObject.second._13),
						TVector3(mapObject.second._21, mapObject.second._22, mapObject.second._23),
						TVector3(mapObject.second._31, mapObject.second._32, mapObject.second._33));
	
					bool isPass = false;
					if (!m_InstanceObjList.empty())
					{
						for (auto& inst : m_InstanceObjList)
						{
							if (inst.second->m_szName == objName)
							{
								INSTANCEDATA data;
								data.matWorld = obj->m_SRTMat;
								inst.second->m_InstanceList.first.push_back(obj);
								inst.second->m_InstanceList.second.push_back(data);
								isPass = true;
							}
						}
					}
					if (!isPass)
					{
						shared_ptr<InstanceObject> instObj = make_shared<InstanceObject>();
						instObj->m_InstanceList.first.push_back(obj);
						instObj->m_szName = obj->m_pFbxObj->m_csName;
						INSTANCEDATA data;
						data.matWorld = obj->m_SRTMat;
						instObj->m_InstanceList.second.push_back(data);
						instObj->CreateInstanceBuffer(obj->m_SRTMat);
						m_InstanceObjList.insert(make_pair((UINT)list.get(), instObj));
	
					}
	
					break;
				}
			}
		}
	
	
	
	
	
	
	}
	if (m_bImguiSave&& !filePathName.empty())
	{
		m_bImguiSave = false;
		m_SaveLoader.SaveMap(m_pQuadTree, filePathName, m_InstanceObjList);
		CTextureMgr::GetInstance().SaveFile(m_pQuadTree->m_pTextureList[0]->m_csName.c_str(), m_pQuadTree->m_pMap->m_pRoughnessLookUpTex.Get());
	}


}
//void MapToolScene::CreateDSS()
//{
//	D3D11_DEPTH_STENCIL_DESC DSVDesc;
//	ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
//	DSVDesc.DepthEnable = TRUE;
//	DSVDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	DSVDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//
//
//	DSVDesc.StencilEnable = FALSE;
//	DSVDesc.StencilReadMask = 1;
//	DSVDesc.StencilWriteMask = 1;
//	DSVDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	DSVDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
//	DSVDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	DSVDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
//
//	DSVDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	DSVDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	DSVDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	DSVDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
//
//	CoreInterface::g_pDevice->CreateDepthStencilState(&DSVDesc, m_pDSS.GetAddressOf());
//}
void MapToolScene::SelectMenu()
{

	{
		ImVec2 ivMin = { static_cast<float>(gWindow_Width) - static_cast<float>(gWindow_Width) / 4.3f, 0 };
		ImGui::SetNextWindowPos(ivMin);
		ImGui::Begin("MapTool Menu");

		if (ImGui::RadioButton("Height", (m_MenuState == MenuState::Height)))
		{
			m_MenuState = MenuState::Height;
		}ImGui::SameLine();
		if (ImGui::RadioButton("Texture", (m_MenuState == MenuState::Texture)))
		{
			m_MenuState = MenuState::Texture;

		}ImGui::SameLine();
		if (ImGui::RadioButton("Object", (m_MenuState == MenuState::Object)))
		{
			m_MenuState = MenuState::Object;
		}


		if (m_MenuState == MenuState::Height)
		{
			ImGui::SeparatorText("BrushType");
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"HeightBrushstrength");
			ImGui::SliderFloat("HeightBrushstrength", (float*)&m_pQuadTree->m_HeightBrushstrength, 1.0f, 50.0f, "%.0f");
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"Brushsize");
			ImGui::SliderFloat("Brushsize", (float*)&m_Select.m_fPickDistance, 1.0f, 20.0f, "%.0f");

			if (ImGui::RadioButton("UP", (m_pQuadTree->m_BrushType == BrushType::UPBRUSH)))
			{
				m_pQuadTree->m_BrushType = BrushType::UPBRUSH;
			}ImGui::SameLine();
			if (ImGui::RadioButton("DOWN", (m_pQuadTree->m_BrushType == BrushType::DOWNBRUSH)))
			{
				m_pQuadTree->m_BrushType = BrushType::DOWNBRUSH;

			}ImGui::SameLine();
			//if (ImGui::RadioButton("SMOOTH", (m_pQuadTree->m_BrushType == BrushType::SMOOTHBRUSH)))
			//{
			//	m_pQuadTree->m_BrushType = BrushType::SMOOTHBRUSH;

			//}ImGui::SameLine();
			if (ImGui::RadioButton("None", (m_pQuadTree->m_BrushType == BrushType::NONE)))
			{
				m_pQuadTree->m_BrushType = BrushType::NONE;

			}
		}
		else if (m_MenuState == MenuState::Texture)
		{
			ImGui::SeparatorText("BrushType");
			ImGui::SliderFloat("Brushstrength", (float*)&m_pQuadTree->m_Brushstrength, 10.0f, 255.0f, "%.0f");
			ImGui::SliderFloat("Brushsize", (float*)&m_Select.m_fPickDistance, 1.0f, 20.0f, "%.0f");
			if (ImGui::RadioButton("Layer1", (m_pQuadTree->m_BrushType == BrushType::LAYER1)))
			{
				m_pQuadTree->m_BrushType = BrushType::LAYER1;

			}ImGui::SameLine();
			if (ImGui::RadioButton("Layer2", (m_pQuadTree->m_BrushType == BrushType::LAYER2)))
			{
				m_pQuadTree->m_BrushType = BrushType::LAYER2;

			}ImGui::SameLine();
			if (ImGui::RadioButton("Layer3", (m_pQuadTree->m_BrushType == BrushType::LAYER3)))
			{
				m_pQuadTree->m_BrushType = BrushType::LAYER3;

			}ImGui::SameLine();
			if (ImGui::RadioButton("Layer4", (m_pQuadTree->m_BrushType == BrushType::LAYER4)))
			{
				m_pQuadTree->m_BrushType = BrushType::LAYER4;

			}
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"Select Texture");
			if (ImGui::BeginListBox("Texturelist"))
			{


				int currindex = static_cast<int>(m_pQuadTree->m_BrushType) - static_cast<int>(BrushType::LAYER1) + 1;
				for (auto& tex : CTextureMgr::GetInstance().m_List)
				{

					if (ImGui::Selectable(wtm(tex.second->m_csName).c_str()) &&
						currindex >= 0 && m_pQuadTree->m_pTextureList.size() > currindex)
					{

						m_pQuadTree->m_pTextureList[currindex] = tex.second;
						m_pQuadTree->m_pTexSRV[currindex] = tex.second->GetSRV();
						break;
					}


				}


				ImGui::EndListBox();
			}



		}
		else if (m_MenuState == MenuState::Object)
		{
			static  TMatrix tempMat;

			if (ImGui::RadioButton("SelectObejct", (m_pQuadTree->m_BrushType == BrushType::OBJECTSELECT)))
			{
				m_pQuadTree->m_BrushType = BrushType::OBJECTSELECT;

			}
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"Select Object");

			if (ImGui::BeginListBox("Objectlist"))
			{
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"Objectlist");
				int index = 0;
				for (auto& obj : ObjList)
				{
					if (ImGui::RadioButton(wtm(obj->m_csName).c_str(),
						(static_cast<int>(m_pQuadTree->m_BrushType) == static_cast<int>(BrushType::OBJECT1) + index)))
					{
						m_pQuadTree->m_BrushType = (BrushType)(static_cast<int>(BrushType::OBJECT1) + index);
					}
					++index;
				}
				ImGui::EndListBox();
			}

			if (ImGui::BeginListBox("PlacedObjectlist"))
			{
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"PlacedObjectlist");
				for (auto& inst : m_InstanceObjList)
				{
					int iobj = 0;
					for (auto& obj : inst.second->m_InstanceList.first)
					{
						if (ImGui::Selectable(wtm(obj->m_MapObjName).c_str()))
						{
							m_SelectInstanceObject = inst.second;
							if (m_SelectInstanceObject != nullptr)
							{
								m_SelectInstanceObject->m_InstanceList.second[m_BeforeKey].Emissive = 0.0f;
								m_SelectInstanceObject->m_InstanceList.second[iobj].Emissive = 1.0f;
								
							}
							m_SelectInstanceObject->m_bBeforeSelect = true;
							m_SelectMapObj = obj;
							m_BeforeKey = iobj;
						}
						if (m_SelectInstanceObject != nullptr && m_SelectInstanceObject->m_bBeforeSelect)
						{
							tempMat._11 = m_SelectMapObj->m_Scale._11;
							tempMat._12 = m_SelectMapObj->m_Scale._22;
							tempMat._13 = m_SelectMapObj->m_Scale._33;
							tempMat._21 = m_SelectMapObj->m_Rotation.x;
							tempMat._22 = m_SelectMapObj->m_Rotation.y;
							tempMat._23 = m_SelectMapObj->m_Rotation.z;
							tempMat._31 = m_SelectMapObj->m_Transform._41;
							tempMat._32 = m_SelectMapObj->m_Transform._42;
							tempMat._33 = m_SelectMapObj->m_Transform._43;
							m_SelectInstanceObject->m_bBeforeSelect = false;
						}
						++iobj;
					}
					
				}
				ImGui::EndListBox();
			}

			if (ImGui::BeginListBox("ObjectData"))
			{
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), u8"ObjectData");


				if (m_SelectInstanceObject != nullptr)
				{

					if (ImGui::BeginListBox("Scale"))
					{
						ImGui::InputFloat("X ##Scale", &tempMat._11);
						ImGui::InputFloat("Y ##Scale", &tempMat._12);
						ImGui::InputFloat("Z ##Scale", &tempMat._13);
						if (ImGui::Button("Conform", ImVec2(60, 30)))
						{
							m_RedoUndo.SaveUndoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup, m_InstanceObjList);
							m_SelectMapObj->m_Scale._11 = tempMat._11;
							m_SelectMapObj->m_Scale._22 = tempMat._12;
							m_SelectMapObj->m_Scale._33 = tempMat._13;
							m_SelectMapObj->m_bIsUpdate = true;
						}
						ImGui::EndListBox();
					}
					if (ImGui::BeginListBox("Rocation"))
					{
						ImGui::InputFloat("X ##Rocation", &tempMat._21);
						ImGui::InputFloat("Y ##Rocation", &tempMat._22);
						ImGui::InputFloat("Z ##Rocation", &tempMat._23);
						if (ImGui::Button("Conform", ImVec2(60, 30)))
						{
							m_RedoUndo.SaveUndoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup, m_InstanceObjList);
							m_SelectMapObj->m_Rotation.x = tempMat._21;
							m_SelectMapObj->m_Rotation.y = tempMat._22;
							m_SelectMapObj->m_Rotation.z = tempMat._23;
							m_SelectMapObj->m_bIsUpdate = true;
						}
						ImGui::EndListBox();
					}

					if (ImGui::BeginListBox("Transform"))
					{
						ImGui::InputFloat("X ##position", &tempMat._31);
						ImGui::InputFloat("Y ##position", &tempMat._32);
						ImGui::InputFloat("Z ##position", &tempMat._33);
						if (ImGui::Button("Conform", ImVec2(60, 30)))
						{
							m_RedoUndo.SaveUndoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup, m_InstanceObjList);
							m_SelectMapObj->m_Transform._41 = tempMat._31;
							m_SelectMapObj->m_Transform._42 = tempMat._32;
							m_SelectMapObj->m_Transform._43 = tempMat._33;
							m_SelectMapObj->m_bIsUpdate = true;
						}
						ImGui::EndListBox();
					}

				}
				if (ImGui::Button("Conform", ImVec2(60, 30)))
				{
					if (m_SelectMapObj != nullptr)
					{
						m_RedoUndo.SaveUndoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup, m_InstanceObjList);
						m_SelectMapObj->m_Scale._11 = tempMat._11;
						m_SelectMapObj->m_Scale._22 = tempMat._12;
						m_SelectMapObj->m_Scale._33 = tempMat._13;
						m_SelectMapObj->m_Rotation.x = tempMat._21;
						m_SelectMapObj->m_Rotation.y = tempMat._22;
						m_SelectMapObj->m_Rotation.z = tempMat._23;
						m_SelectMapObj->m_Transform._41 = tempMat._31;
						m_SelectMapObj->m_Transform._42 = tempMat._32;
						m_SelectMapObj->m_Transform._43 = tempMat._33;
						m_SelectMapObj->m_bIsUpdate = true;
					}

				}
				ImGui::EndListBox();
			}
		}
		if (ImGui::Button("DeleteObject", ImVec2(90, 30)))
		{
			if (m_SelectMapObj)
			{
				for (auto& Instance : m_InstanceObjList)
				{
					int iobj = 0;
					for (auto& obj : Instance.second->m_InstanceList.first)
					{
						if (obj == m_SelectMapObj)
						{

							Instance.second->m_InstanceList.first[iobj] = nullptr;
							Instance.second->m_InstanceList.first[iobj] = Instance.second->m_InstanceList.first[Instance.second->m_InstanceList.first.size()-1];
							Instance.second->m_InstanceList.first.resize(Instance.second->m_InstanceList.first.size() - 1);
							Instance.second->m_InstanceList.second[iobj] = Instance.second->m_InstanceList.second[Instance.second->m_InstanceList.second.size() - 1];
							Instance.second->m_InstanceList.second.resize(Instance.second->m_InstanceList.second.size() - 1);
							m_SelectInstanceObject = nullptr;
							m_SelectMapObj = nullptr;
							break;
						}
						iobj++;
					}
					
					for (vector<shared_ptr<MapObject>>::iterator iter = Instance.second->m_InstanceList.first.begin();
						iter != Instance.second->m_InstanceList.first.end();
						++iter)
					{
						if (iter->get() == m_SelectMapObj.get())
						{
							
							Instance.second->m_InstanceList;

							break;
						}
						
					}
				}
			}

		}

		if (ImGui::Button("Undo", ImVec2(50, 30)))
		{

			if (m_RedoUndo.UndoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup, m_InstanceObjList))
			{
				CoreInterface::g_pImmediateContext->UpdateSubresource(m_pHeightMap->m_pVertexBuffer.Get(),
					0, NULL, &m_pHeightMap->m_vVertexList.at(0), 0, 0);
				UINT const DataSize = sizeof(BYTE) * 4;
				UINT const RowPitch = DataSize * m_pHeightMap->m_iCol;
				CoreInterface::g_pImmediateContext->UpdateSubresource(m_pHeightMap->m_pRoughnessLookUpTex.Get(), 0, NULL, m_pHeightMap->m_fLookup, RowPitch, 0);
				for (auto& node : m_pQuadTree->m_vLeafNodeList)
				{
					m_pQuadTree->UpdateVertexList(node);
					CoreInterface::g_pImmediateContext->UpdateSubresource(node->m_pVertexBuffer.Get(),
						0, NULL, &node->m_vVertexList.at(0), 0, 0);
				}
			}


		}ImGui::SameLine();
		if (ImGui::Button("Redo", ImVec2(50, 30)))
		{

			if (m_RedoUndo.RedoData(m_pHeightMap->m_vVertexList, m_pHeightMap->m_fLookup,m_InstanceObjList))
			{
				CoreInterface::g_pImmediateContext->UpdateSubresource(m_pHeightMap->m_pVertexBuffer.Get(),
					0, NULL, &m_pHeightMap->m_vVertexList.at(0), 0, 0);
				UINT const DataSize = sizeof(BYTE) * 4;
				UINT const RowPitch = DataSize * m_pHeightMap->m_iCol;
				CoreInterface::g_pImmediateContext->UpdateSubresource(m_pHeightMap->m_pRoughnessLookUpTex.Get(), 0, NULL, m_pHeightMap->m_fLookup, RowPitch, 0);
				for (auto& node : m_pQuadTree->m_vLeafNodeList)
				{
					m_pQuadTree->UpdateVertexList(node);
					CoreInterface::g_pImmediateContext->UpdateSubresource(node->m_pVertexBuffer.Get(),
						0, NULL, &node->m_vVertexList.at(0), 0, 0);
				}
			}

		}

		ImGui::End();

	}
}

void MapToolScene::DrawReflectMap(int num ,TMatrix* world, TMatrix* view, TMatrix* proj)
{

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	CoreInterface::g_pImmediateContext->PSSetShaderResources(0, 1, pSRV);
	CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, pSRV);
	CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 1, pSRV);
	CoreInterface::g_pImmediateContext->VSSetConstantBuffers(4, 1, m_pFogCB.GetAddressOf());
	CoreInterface::g_pImmediateContext->PSSetConstantBuffers(4, 1, m_pFogCB.GetAddressOf());
	if (m_WaterMap.m_WaterEffectRT[num].Begin(TVector4(1, 1, 1, 1)))
	{
		m_pQuadTree->m_pMap->SetMatrix(world, view, proj);
		m_pQuadTree->m_pMap->PreRender();
		UINT Stride = sizeof(TVector3);
		UINT offset = 0;
		CoreInterface::g_pImmediateContext->IASetVertexBuffers(1, 1, m_pQuadTree->m_pMap->m_pTangentVB.GetAddressOf(), &Stride, &offset);//p 159참조
		CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, m_pQuadTree->m_pTexSRV[0].GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 4, m_pQuadTree->m_pTexSRV[1].GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetShaderResources(6, 4, m_pQuadTree->m_pNormalMapSRV[0].GetAddressOf());
		for (auto node : m_pQuadTree->m_vLeafNodeList)
		{
			UINT Stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;

			CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 1, node->m_pVertexBuffer.GetAddressOf(), &Stride, &offset);//p 159참조
			CoreInterface::g_pImmediateContext->IASetIndexBuffer(node->m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			CoreInterface::g_pImmediateContext->DrawIndexed(node->m_vIndexList.size(), 0, 0);


		}


		for (auto& inst : m_InstanceObjList)
		{
			inst.second->Render(world, view, proj);
		}


		m_pSkyBox->SetMatrix(world, view, proj);
		m_pSkyBox->Render();
		m_WaterMap.m_WaterEffectRT[num].End();
	}

}

