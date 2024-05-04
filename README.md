## 실행 시 주의사항
-FBXSD/lib/release/libfbxsdk-md

-FBXSD/lib/release/libfbxsdk-mt
파일 추가시 빌드가능

-Release 빌드필요

## DirectX11 3D MapTool
  
DirectX11 을 이용한 맵툴제작
* 제작기간 : 2023.12 ~ 2024.02

* 시연 영상 : https://www.youtube.com/watch?v=LpMHkSxpFdY
 
* DownloadLink : https://drive.google.com/file/d/1-WgzNXx6OKWGYwIPq4wMHWlw6BUvFI9R/view?usp=drive_link
  
## 조작법

* W,A,S,D - 이동
* Q - 상승
* E  - 하강
* 마우스 좌드래그 - 화면 이동
* 마우스 우클릭  - 오브젝트 배치 및 스플래팅  

## 지형조작

![Alt text](ReadmeImage/1.gif)
* 마우스 피킹을 이용하여 선택된 지점의 지형을 올리기 및 내리기가 가능합니다.

<details>
<summary>코드샘플</summary>

```c++
void CQuadTree::FindNeighborVertex(CNode* node, Circle circle)
{
    //브러쉬에 따라 업브러쉬인지 다운브러쉬인지 
    if (m_BrushType == BrushType::UPBRUSH)
    {
        float newHeight = 0.0f;
        float maxHeight = 0.0f;
        //선택된 노드의 정점들을 확인하여 피킹된 점을 거리에 따라 높낮이를 다르게
        for (int ivtx = 0; ivtx < node->m_vVertexList.size(); ++ivtx)
        {
            TVector2 center(circle.vCenter.x, circle.vCenter.y);
            TVector2 pt(node->m_vVertexList[ivtx].Pos.x, node->m_vVertexList[ivtx].Pos.z);
            float dist = (center - pt).Length();
            //원의 크기보다 정점의 거리가 멀면 다음 정점으로 넘어간다.
            if (circle.fRadius <= dist)
                continue;
           //이대로 사용하면 가까운 점은 0 먼점은 1 이 나오기 때문에 
           //가까운 곳을 1로 먼 곳을 0으로 만들어준다
            dist = ( dist / circle.fRadius )-1;
            dist *= -1;
            //델타 타임을 사용하여 고정된 값이 상승하게 구현
            node->m_vVertexList[ivtx].Pos.y += dist* g_fSecondPerFrame * m_HeightBrushstrength;
            newHeight = node->m_vVertexList[ivtx].Pos.y;
            //노드의 바운딩 박스를 새로 계산하기 위해 높이값을 변경
            if (newHeight > maxHeight)
            {
                maxHeight = newHeight;
                node->m_Box.m_height = maxHeight;
            }
               
        }

    }
```
</details>

## Undo/Redo  
![Alt text](ReadmeImage/2.gif)
* 작업한 내용을 vector로 저장하여 Undo 및 Redo기능 사용 가능합니다.

<details>
<summary>SaveData코드샘플</summary>

```c++
void RedoUndo::SaveUndoData(std::vector<PNCT_VERTEX> ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>objlist)
{
    TempSaveData temp;
    //정점의 정보를 임시로 저장해 놓을 데이터에 넣어놓는다.
    if (!ver.empty())
    {
        temp.TempVertexList.resize(ver.size());
        std::copy(ver.begin(), ver.end(), temp.TempVertexList.begin());
    }
    //스플레팅 데이터
    if (pixeldata != NULL)
    {
        int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
        temp.TempPixelDataList = new BYTE[iSize];
        memcpy_s(temp.TempPixelDataList, iSize, pixeldata, iSize);
    }
    //오브젝트 데이터
    temp.TempInstanceObjList = objlist;
    for (auto& obj : objlist)
    {
        temp.TempMapObjList.insert(make_pair(obj.second, obj.second->m_InstanceList.first));
        temp.TempInstanceDataList.insert(make_pair(obj.second, obj.second->m_InstanceList.second));
    }
    //인스턴싱 데이터
    for (auto& inst : temp.TempInstanceObjList)
    {
        for (auto& obj : inst.second->m_InstanceList.first)
        {
            TMatrix mat;
            //scale
            mat._11 = obj->m_Scale._11;  mat._12 = obj->m_Scale._22; mat._13 = obj->m_Scale._33;
            //rotate
            mat._21 = obj->m_Rotation.x;  mat._22 = obj->m_Rotation.y;  mat._23 = obj->m_Rotation.z;
            //trasform
            mat._31 = obj->m_Transform._41;  mat._32 = obj->m_Transform._42;  mat._33 = obj->m_Transform._43;
            temp.TempObjectMatList.insert(make_pair(obj->m_MapObjName, mat));
        }
       
    }
    //되돌리는 목록에 추가해준다
    m_vUndoList.push_back(temp);

}
```
</details>


<details>
<summary>LoadData코드샘플</summary>

```c++

bool RedoUndo::UndoData(std::vector<PNCT_VERTEX>& ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>&objlist)
{
    //리스트가 비어있다면 실행하지 않는다.
    if (m_vUndoList.empty())
        return false;
    //작업내용을 취소하기 전에 되돌릴 데이터를 저장해 놓는다.
    SaveRedoData(ver, pixeldata,objlist);
    //리스트의 가장 최신 데이터를 가져온다.
    auto list = m_vUndoList.back();
    //각각의 정보들을 현재 데이터에 덮어 씌운다.
    if (!list.TempVertexList.empty())
        std::copy(list.TempVertexList.begin(), list.TempVertexList.end(), ver.begin());
    int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
    if (list.TempPixelDataList != NULL)
        memcpy_s(pixeldata, iSize, list.TempPixelDataList, iSize);


    for (auto& node : m_pQuadTree->m_vLeafNodeList)
    {
        m_pQuadTree->UpdateVertexList(node);
    }
    objlist = list.TempInstanceObjList;
    for (auto& obj : objlist)
    {
        auto objlist = list.TempMapObjList.find(obj.second);
        auto datalist = list.TempInstanceDataList.find(obj.second);
        obj.second->m_InstanceList.first = objlist->second;
        obj.second->m_InstanceList.second = datalist->second;

    }
   
    


    for (auto& inst : objlist)
    {

        for (auto& obj : inst.second->m_InstanceList.first)
        {
            auto& objName = obj->m_MapObjName;
            for (auto& mat : list.TempObjectMatList)
            {
                if (mat.first == objName)
                {
                    obj->m_Scale._11 = mat.second._11;
                    obj->m_Scale._22 = mat.second._12;
                    obj->m_Scale._33 = mat.second._13;
                    obj->m_Rotation.x = mat.second._21;
                    obj->m_Rotation.y = mat.second._22;
                    obj->m_Rotation.z = mat.second._23;
                    obj->m_Transform._41 = mat.second._31;
                    obj->m_Transform._42 = mat.second._32;
                    obj->m_Transform._43 = mat.second._33;
                    break;
                }
            }
        }
    }

    //사용한 임시데이터 들은 삭제해준다.
    if (!list.TempVertexList.empty())
        list.TempVertexList.clear();
    if (list.TempPixelDataList != NULL)
        delete[] list.TempPixelDataList;
    if (!list.TempInstanceObjList.empty())
        list.TempInstanceObjList.clear();
    if (!list.TempObjectMatList.empty())
        list.TempObjectMatList.clear();
    if (!list.TempInstanceDataList.empty());
        list.TempInstanceDataList.clear();
    if (!list.TempMapObjList.empty());
         list.TempMapObjList.clear();

    m_vUndoList.pop_back();
    return true;
}

```
</details>


## Splatting
![Alt text](ReadmeImage/3.gif)
* 멀티 텍스쳐링을 이용하여 선택된 지점의 픽셀 컬러를 원하는 텍스쳐의 컬러로 드로잉 가능합니다.

<details>
<summary>Splatting코드샘플</summary>

```c++

void CQuadTree::UpdateVertexAlpha(Circle circle)
{
    //피킹 지점으로 부터 원을 만들어 가져온다.
    //노드에서 각각 정점을 가져오면 중복되는 정점이 생겨 격자가 생기기 때문에
    //스플래팅을할 노드들을 중복없이 가져와 사용해야한다.
    for (auto idx : m_ChangeAlphaList)
    {
        TVector2 center(circle.vCenter.x, circle.vCenter.y);
        TVector2 pt(m_vVertexList[idx].Pos.x, m_vVertexList[idx].Pos.z);
        float dist = (center - pt).Length();

        if (circle.fRadius <= dist)
            continue;

        dist = (dist / circle.fRadius) - 1;
        dist *= -1;
        //높이 올리기 때와 마찬가지로 값을 구한 후 맵의 알파맵의 데이터를 가져와
        //레이어에 맞게 값을 변경시켜준다.

        BYTE* pixel = &m_pMap->m_fLookup[idx * 4];
        int brushType = 0;
        if (m_BrushType == BrushType::LAYER1)
            brushType = 0;
        if (m_BrushType == BrushType::LAYER2)
            brushType = 1;
        if (m_BrushType == BrushType::LAYER3)
            brushType = 2;
        if (m_BrushType == BrushType::LAYER4)
            brushType = 3;
        
        if (pixel[brushType] + dist * m_Brushstrength * g_fSecondPerFrame * 50 <= 255)
        {         
            pixel[brushType] += dist * m_Brushstrength * g_fSecondPerFrame * 50;
        }         
        else      
        {         
            pixel[brushType] = 255;
        }

    }
    //변경된 값을 셰이더로 넘겨준다.
    UINT const DataSize = sizeof(BYTE) * 4;
    UINT const RowPitch = DataSize * m_Width;
    CoreInterface::g_pImmediateContext->UpdateSubresource(m_pMap->m_pRoughnessLookUpTex.Get(), 0, NULL, m_pMap->m_fLookup, RowPitch, 0);
    m_ChangeAlphaList.clear();
}

```
</details>


<details>
<summary>Splatting셰이더코드샘플</summary>

```c++
// 기본 맵의 텍스쳐과 스플래팅 데이터가 저장된 알파맵의 텍스쳐를 가져온다
    float4 vDetail = TexDifuse1.Sample(Sample0, vIn.t1);
    float4 mask = TexDifuse2.Sample(Sample0, vIn.t);
    //스플래팅에 사용할 텍스쳐를 가져온다
    float4 vColor = TexDifuse3[0].Sample(Sample0, vIn.t1);
    float4 vColor1 = TexDifuse3[1].Sample(Sample0, vIn.t1);
    float4 vColor2 = TexDifuse3[2].Sample(Sample0, vIn.t1);
    float4 vColor3 = TexDifuse3[3].Sample(Sample0, vIn.t1); 
      
//알파맵에 저장된 값을 사용할 텍스쳐의 픽셀값으로 보간해준다
    float4 vFinalColor = lerp(vDetail, vColor , mask.x);
    vFinalColor = lerp(vFinalColor, vColor1,  mask.y);
    vFinalColor = lerp(vFinalColor, vColor2,  mask.z);
    vFinalColor = lerp(vFinalColor, vColor3,  mask.w);

```
</details>


## 오브젝트 배치
![Alt text](ReadmeImage/4.gif)
* 원하는 오브젝트를 마우스 피킹 지점에 배치 가능합니다.

<details>
<summary>Object배치 샘플코드</summary>

```c++

		for (auto& node : m_pQuadTree->DrawNodeList)
		{
			//교점 확인
			if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_PUSH && m_Select.GetIntersectionBox(&node->m_Box))
			{
				//어떤 FBX 오브젝트를 불러올지 선택
				int currindex = static_cast<int>(m_pQuadTree->m_BrushType) - static_cast<int>(BrushType::OBJECT1);
				auto obj = std::make_shared<MapObject>();
				//맵 오브젝트 생성
				obj->SetMapObj(ObjList[currindex].get(), ObjList[currindex]->m_AnimInfo);
				//위치를 교점 기준으로 하되 y값은 지형의 최대y값을 기준으로 배치한다
				obj->m_Transform._41 = m_Select.m_vIntersection.x;
				obj->m_Transform._42 = node->m_Box.m_Max.y;
				obj->m_Transform._43 = m_Select.m_vIntersection.z;
				//오브젝트 리스트를 구분하기 위해 이름이 중복되는지 확인한 후 중보된다면 이름뒤에 #N 을 부여한다
				obj->m_MapObjName = obj->m_pFbxObj->m_csName;
				m_pQuadTree->CheckDuplicationName(obj->m_MapObjName);
				m_pQuadTree->m_MapObjList.insert(make_pair(obj->m_MapObjName, obj));

				//인스턴싱을 위해 리스트에서 찾는다
				auto instanceObj = m_InstanceObjList.find((UINT)ObjList[currindex].get());
				//인스턴싱 데이터가 없다면 처음 만들어진 오브젝트 이므로 새로 생성해준다.
				if (instanceObj == m_InstanceObjList.end())
				{
					shared_ptr<InstanceObject> instObj = make_shared<InstanceObject>();
					instObj->m_InstanceList.first.push_back(obj);
					//인스턴싱에 필요한 데이터를 채워넣는다.
					INSTANCEDATA data;
					data.matWorld = obj->m_SRTMat;
					instObj->m_InstanceList.second.push_back(data);
					instObj->CreateInstanceBuffer(obj->m_SRTMat);
					m_InstanceObjList.insert(make_pair((UINT)ObjList[currindex].get(), instObj));
				}
				else
				{
					//데이터가 있다면 인스턴싱에 필요한 데이터만 넘겨준다
					instanceObj->second->m_szName = obj->m_pFbxObj->m_csName;
					INSTANCEDATA data;
					data.matWorld = obj->m_SRTMat;
					instanceObj->second->m_InstanceList.first.push_back(obj);
					instanceObj->second->m_InstanceList.second.push_back(data);

				}


			}

```
</details>

## 오브젝트 선택,삭제,SRT조정
![Alt text](ReadmeImage/5.gif)
* 오브젝트를 선택하여 삭제 및 SRT를 조정 가능합니다.
  
<details>
<summary>오브젝트 선택 샘플코드</summary>

```c++
	if (m_pQuadTree->m_BrushType == BrushType::OBJECTSELECT)
	{
		if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_PUSH)
		{	//인스턴싱된 오브젝트를 찾는다
			for (auto& instobj : m_InstanceObjList)
			{
				for (int iobj = 0 ; iobj < instobj.second->m_InstanceList.first.size(); ++iobj)
				{	
					auto& obj = instobj.second->m_InstanceList.first[iobj];
					for (auto& child : obj->m_vChildList)
					{
						//오브젝트의 자식리스트를 순회하며 박스가 클릭되었는지 확인한다.
						if (m_Select.GetIntersectionBox(&child->m_CollisionBox))
						{
							//SRT값을 가져오기위해 선택한 오브젝트를 설정해 준다.
							if (m_SelectInstanceObject != nullptr)
							{
								//이전에 선택한 오브젝트가 있는경우 
								m_SelectInstanceObject->m_bBeforeSelect = false;
								m_SelectInstanceObject->m_InstanceList.second[m_BeforeKey].Emissive = 0.0f;
								
								m_SelectInstanceObject = instobj.second;
								m_SelectMapObj = obj;
								m_SelectInstanceObject->m_bBeforeSelect = true;
								//선택 했는지 시각적으로 보여주기 위해 값을 넣어준다.
								instobj.second->m_InstanceList.second[iobj].Emissive = 1.0f;
								m_BeforeKey = iobj;
							}
							else
							{
								//선택한 오브젝트가 없었던경우
								m_SelectInstanceObject = instobj.second;
								m_SelectMapObj = obj;
								m_SelectInstanceObject->m_bBeforeSelect = true;
								instobj.second->m_InstanceList.second[iobj].Emissive = 1.0f;
								m_BeforeKey = iobj;
							}	
							//중복해서 선택하는것을 방지하기위해 반복문을 탈출한다.
							break;
						}
					}

				}
			}
		}
	}


```
</details>

<details>
<summary>오브젝트 삭제 샘플코드</summary>

```c++
for (auto& Instance : m_InstanceObjList)
{
	int iobj = 0;
	for (auto& obj : Instance.second->m_InstanceList.first)
	{
		if (obj == m_SelectMapObj)
		{
			//인스턴스 리스트의 오브젝트와 선택한 오브젝트가 동일한경우 
			//벡터는 중간의 값을 삭제하는 경우 속도가 느리기 때문에 
			//삭제할 데이터와 벡터의 맨 뒤의 데이터를 교체한다.
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
}


```

</details>


<details>
<summary>오브젝트 SRT 조정 샘플코드</summary>

```c++
//확인 버튼을 누르면 임시로 저장한 SRT 값을 선택한 오브젝트로 바꿔준다.
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
```
</details>

## NewMap 생성
![Alt text](ReadmeImage/6.gif)
* 원하는 사이즈를 입력하여 새로운 맵을 생성 할 수 있습니다.

<details>
<summary>맵 생성 샘플코드</summary>

```c++

if (OpenNew)
	{
		static float Mapsize = 0;
		ImGui::InputFloat("MapSize", &Mapsize);
		if (ImGui::Button("Conform", ImVec2(60, 30)))
		{
			//기존의 맵데이터들을 전부 삭제 후 재 생성한다.
			m_pHeightMap.reset();
			m_pQuadTree.reset();
			m_InstanceObjList.clear();

			m_pHeightMap = make_shared<CHeightMap>();
			m_pHeightMap->Init(L"", Mapsize);

			m_pQuadTree = std::make_shared<CQuadTree>();
			m_pQuadTree->BuildQuadTree(m_pHeightMap, m_pHeightMap->m_iRow, m_pHeightMap->m_iCol, m_pHeightMap->m_vVertexList);


			m_pWaterMap = nullptr;
			m_pWaterMap = new WaterMap;
			m_pWaterMap->Init();
			CMapDesc WaterMapDesc = { m_pHeightMap->m_iRow,
				m_pHeightMap->m_iCol,
				1.0f, 0.0f,
				L"../../Resource/water.bmp",
				L"../../Resource/WaterMap.hlsl" };
			m_pWaterMap->Load(WaterMapDesc);


			OpenNew = false;
		}


	}

```
</details>

## Load HeightMap
![Alt text](ReadmeImage/8.gif)
* 원하는 높이맵을 불러와 맵에 적용 가능합니다.

<details>
<summary>HeightMap Load 샘플코드</summary>

```c++

bool CHeightMap::CreateHeightMap(std::wstring HeightMapName,int size)
{
	if (!HeightMapName.empty())
	{
		HRESULT hr;
		size_t maxsize = 0;
		auto ImageObj = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata mData;
		//높이맵 텍스쳐의 데이터를 가져온다
		hr = DirectX::GetMetadataFromWICFile(HeightMapName.c_str(), DirectX::WIC_FLAGS_NONE, mData);
		if (SUCCEEDED(hr))
		{
			hr = DirectX::LoadFromWICFile(HeightMapName.c_str(), DirectX::WIC_FLAGS_NONE, &mData, *ImageObj);
			if (SUCCEEDED(hr))
			{

			}
		}
		//정사각형이 아닌경우 강제로 정사각형으로 만들어준다.
		if (!CheckSquare(mData.width - 1))
		{
			mData.width = ResizeMap(mData.width);

		}
		if (!CheckSquare(mData.height - 1))
		{
			mData.height = ResizeMap(mData.height);

		}
		m_fHeightList.resize(mData.height * mData.width);
	// 가져온 높이맵의 픽셀 데이터를 가져와 가져온 값을 높이 리스트에 넣어준다.
		UCHAR* pTexels = (UCHAR*)ImageObj->GetImages()->pixels;
		PNCT_VERTEX	v;
		for (UINT row = 0; row < mData.height; row++)
		{
			UINT rowStart = row * ImageObj->GetImages()->rowPitch;
			for (UINT col = 0; col < mData.width; col++)
			{
				UINT colStart = col * 4;
				UINT uRed = pTexels[rowStart + colStart + 0];
				m_fHeightList[row * mData.width + col] = (float)uRed * 0.3;	/// DWORD이므로 pitch/4	
			}
		}


		m_iRow = mData.height;
		m_iCol = mData.width;



	}
	else
	{
		//높이맵이 없는경우 0으로 초기화 해준다
		if (!CheckSquare(size - 1))
		{
			size = ResizeMap(size);

		}
		if (!CheckSquare(size - 1))
		{
			size = ResizeMap(size);

		}
		m_fHeightList.resize(size * size);

		PNCT_VERTEX	v;
		for (UINT row = 0; row < size; row++)
		{
			for (UINT col = 0; col < size; col++)
			{
				m_fHeightList[row * size + col] = 0;	/// DWORD이므로 pitch/4	
			}
		}


		m_iRow = size;
		m_iCol = size;



	}

	return true;
}

```

</details>

## LoadMap
![Alt text](ReadmeImage/9.gif)
* 이전에 저장해 놓았던 맵 정보를 불러옵니다.
* c++ 의 파일 입출력을 사용
<details>
<summary>LoadMap 샘플코드</summary>

```c++

if (m_bImguiLoad && !filePathName.empty())
	{
		m_bImguiLoad = false;
		//맵 정보를 파일로 부터 불러온다
		m_SaveLoader.LoadMap(filePathName,m_InstanceObjList);
		//기존의 맵데이터를 리셋 시키고 로드한 데이터를 기준으로 새로 생성한다.
		m_pHeightMap.reset();
		m_pHeightMap = make_shared<CHeightMap>();
		m_pQuadTree->m_pTexSRV[0].Reset();
		m_pQuadTree.reset();
		m_pQuadTree = make_shared<CQuadTree>();
		m_InstanceObjList.clear();
		m_pHeightMap->Init(L"", m_SaveLoader.m_Mapsize[0]);
		m_pHeightMap->m_bStaticLight = true;
		CMapDesc MapDesc = { m_SaveLoader.m_Mapsize[0],  m_SaveLoader.m_Mapsize[1],1.0f,1.0f,m_SaveLoader.m_SubTexture[0].c_str(),L"../../Resource/MultiTex.hlsl"};
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
		//스플레팅 데이터 
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
		// 인스턴싱한 오브젝트의 데이터
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
		//워터맵 생성
		m_pWaterMap=nullptr;
		m_pWaterMap = new WaterMap;
		m_pWaterMap->Init();
		CMapDesc WaterMapDesc = { m_pHeightMap->m_iRow,
			m_pHeightMap->m_iCol,
			1.0f, 0.0f,
			L"../../Resource/water.bmp",
			L"../../Resource/WaterMap.hlsl" };
		m_pWaterMap->Load(WaterMapDesc);
	
	
	
	
	}

```
</details>

## SaveMap
![Alt text](ReadmeImage/10.gif)
* 현재 맵의 정보를 저장합니다.
* c++ 의 파일 입출력을 사용
<details>
<summary>SaveMap 샘플코드</summary>

```c++
bool SaveLoader::SaveMap(shared_ptr<CQuadTree> tree, std::string filename, std::map<UINT, shared_ptr<InstanceObject>>list)
{
	FILE* fpWrite = nullptr;
	if (fopen_s(&fpWrite, filename.c_str(), "w") == 0)
	{
		bool bRet = true;
		//사이즈 저장
		std::string header = "#MapSize";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%d\t", tree->m_pMap->m_iCol);
		bRet = fprintf_s(fpWrite, "%d\n", tree->m_pMap->m_iRow);

		//높이값 저장
		header = "#Mapheight";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%d\n", static_cast<int>(tree->m_pMap->m_vVertexList.size()));
		tree->UpdateMapVertexList(); // 노드의 높이를 맵 높이와 같게 만듬
		for (auto& vertex : tree->m_pMap->m_vVertexList)
		{
			bRet = fprintf_s(fpWrite, "%f\n", vertex.Pos.y);//버텍스 높이 
		}

		//오브젝트 인스턴싱 데이터
		for (auto& inst : list)
		{
			header = "#MapObject";
			bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
			bRet = fprintf_s(fpWrite, "%d\n", static_cast<int>(inst.second->m_InstanceList.first.size()));//오브젝트 개수
			for (auto& object : inst.second->m_InstanceList.first)
			{
				bRet = fprintf_s(fpWrite, "%s\t", wtm(object->m_MapObjName).c_str());
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._11);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._22);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._33);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.x);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.y);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.z);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Transform._41);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Transform._42);
				bRet = fprintf_s(fpWrite, "%f\n", object->m_Transform._43);
			}
		}
		//맵의 텍스쳐 정보
		header = "#MapTexture";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" +tree->m_pTextureList[1]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" + tree->m_pTextureList[2]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" + tree->m_pTextureList[3]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\n", wtm(L"../../Resource/" + tree->m_pTextureList[4]->m_csName).c_str());
		header = "#MapAlpha";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		std::string alphafile = wtm(L"../../Resource/" + tree->m_pTextureList[0]->m_csName);
		bRet = fprintf_s(fpWrite, "%s\n", alphafile.c_str());

		fclose(fpWrite);
	}

	return true;
}



```
</details>

## Watermap/Environment Mapping
![Alt text](ReadmeImage/11.gif)
* WaterMap에 환경맵핑을 적용하여 오브젝트 및 지형을 표현가능 합니다.

<details>
<summary>watermap 샘플코드</summary>

```c++

void MapToolScene::DrawReflectMap(int num ,TMatrix* world, TMatrix* view, TMatrix* proj)
{
	//렌더타겟에 워터맵을 기준으로 반사한 모습을 그려준다
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	CoreInterface::g_pImmediateContext->PSSetShaderResources(0, 1, pSRV);
	CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, pSRV);
	CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 1, pSRV);
	CoreInterface::g_pImmediateContext->VSSetConstantBuffers(4, 1, m_pFogCB.GetAddressOf());
	CoreInterface::g_pImmediateContext->PSSetConstantBuffers(4, 1, m_pFogCB.GetAddressOf());
	if (m_pWaterMap->m_WaterEffectRT[num].Begin(TVector4(1, 1, 1, 1)))
	{
		m_pQuadTree->m_pMap->SetMatrix(world, view, proj);
		m_pQuadTree->m_pMap->PreRender();
		UINT Stride = sizeof(TVector3);
		UINT offset = 0;
		CoreInterface::g_pImmediateContext->IASetVertexBuffers(1, 1, m_pQuadTree->m_pMap->m_pTangentVB.GetAddressOf(), &Stride, &offset);
		CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1, m_pQuadTree->m_pTexSRV[0].GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 4, m_pQuadTree->m_pTexSRV[1].GetAddressOf());
		CoreInterface::g_pImmediateContext->PSSetShaderResources(6, 4, m_pQuadTree->m_pNormalMapSRV[0].GetAddressOf());
		for (auto node : m_pQuadTree->m_vLeafNodeList)
		{
			UINT Stride = sizeof(PNCT_VERTEX);
			UINT offset = 0;

			CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 1, node->m_pVertexBuffer.GetAddressOf(), &Stride, &offset);
			CoreInterface::g_pImmediateContext->IASetIndexBuffer(node->m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			CoreInterface::g_pImmediateContext->DrawIndexed(node->m_vIndexList.size(), 0, 0);


		}

		//오브젝트와 스카이박스도 그려준다
		for (auto& inst : m_InstanceObjList)
		{
			inst.second->Render(world, view, proj);
		}


		m_pSkyBox->SetMatrix(world, view, proj);
		m_pSkyBox->Render();
		m_pWaterMap->m_WaterEffectRT[num].End();
	}

}

```
</details>

## Instancing
![Alt text](ReadmeImage/12.gif)
* 오브젝트의 Instancing을 적용하여 많은 수의 오브젝트를 배치 하여도 안정적인 FPS를 유지 가능합니다.

<details>
<summary>Instancing 샘플코드</summary>

```c++
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




```
</details>
