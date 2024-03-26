#include "CQuadTree.h"
#include"CCollision.h"
#include"CInput.h"


void CQuadTree::BuildTree(CNode* pNode)
{
    if (SubDivide(pNode))
    {
        for (int iNode = 0; iNode < pNode->m_pChildList.size(); ++iNode)
        {
            //m_bUsedIndexList
            DWORD dwX = pNode->m_pChildList[iNode]->m_dwPositionIndex[0];
            DWORD dwY = pNode->m_pChildList[iNode]->m_dwPositionIndex[1];

            DWORD dwIndex = dwY * pow(2.0f, (float)pNode->m_pChildList[iNode]->m_iDepth) + dwX;
            m_LevelList[pNode->m_pChildList[iNode]->m_iDepth][dwIndex] = pNode->m_pChildList[iNode];
            BuildTree(pNode->m_pChildList[iNode]);
        }
    }

}
void CQuadTree::FindDrawNode(CNode* pNode)
{
    if (pNode == nullptr)
    {
        return;
    }
    bool collision = false;
    if (CoreInterface::g_pMainCamera->ClassifyOBB(&pNode->m_Box))
    {
        DrawNodeList.push_back(pNode);
        collision = true;

    }
    if (!collision)
    {
        for (int i = 0; i < pNode->m_pChildList.size(); ++i)
        {
            FindDrawNode(pNode->m_pChildList[i]);
        }
    }

   
}
void CQuadTree::UpdateNodeBoundingBox(CNode* pNode, float height)
{
    if (pNode == m_pRootNode)
        return;
    DWORD dwV0 = pNode->m_CornerIndex[0];
    DWORD dwV1 = pNode->m_CornerIndex[3];

    TVector2 vHeight = GetHeightFromNode(pNode);
    pNode->m_Box.m_Max.x = m_pMap->m_vVertexList[dwV1].Pos.x;
    pNode->m_Box.m_Max.z = m_pMap->m_vVertexList[dwV0].Pos.z;
    pNode->m_Box.m_Min.x = m_pMap->m_vVertexList[dwV0].Pos.x;
    pNode->m_Box.m_Min.z = m_pMap->m_vVertexList[dwV1].Pos.z;
    pNode->m_Box.m_Max.y = vHeight.x;
    pNode->m_Box.m_Min.y = vHeight.y;
    pNode->m_Box.m_Max = TVector3(pNode->m_Box.m_Max.x, height, pNode->m_Box.m_Max.z);
    pNode->m_Box.Size.x = (pNode->m_Box.m_Max.x - pNode->m_Box.m_Min.x) / 2.0f;
    pNode->m_Box.Size.y = (pNode->m_Box.m_Max.y - pNode->m_Box.m_Min.y) / 2.0f;
    pNode->m_Box.Size.z = (pNode->m_Box.m_Max.z - pNode->m_Box.m_Min.z) / 2.0f;
    pNode->m_Box.m_pCenter = (pNode->m_Box.m_Max + pNode->m_Box.m_Min) * 0.5f;
    pNode->m_Box.m_fExtent[0] = pNode->m_Box.m_Max.x - pNode->m_Box.m_pCenter.x;
    pNode->m_Box.m_fExtent[1] = pNode->m_Box.m_Max.y - pNode->m_Box.m_pCenter.y;
    pNode->m_Box.m_fExtent[2] = pNode->m_Box.m_Max.z - pNode->m_Box.m_pCenter.z;
    pNode->m_Box.m_Width = pNode->m_Box.m_Max.x - pNode->m_Box.m_Min.x;
    pNode->m_Box.m_height = pNode->m_Box.m_Max.y - pNode->m_Box.m_Min.y;
    pNode->m_Box.m_Depth = pNode->m_Box.m_Max.z - pNode->m_Box.m_Min.z;
    UpdateNodeBoundingBox(pNode->m_pParent, height);
}
void CQuadTree::FindNeighborVertex(CNode* node, Circle circle)
{
    if (m_BrushType == BrushType::UPBRUSH)
    {
        float newHeight = 0.0f;
        float maxHeight = 0.0f;
        for (int ivtx = 0; ivtx < node->m_vVertexList.size(); ++ivtx)
        {
            TVector2 center(circle.vCenter.x, circle.vCenter.y);
            TVector2 pt(node->m_vVertexList[ivtx].Pos.x, node->m_vVertexList[ivtx].Pos.z);
            float dist = (center - pt).Length();

            if (circle.fRadius <= dist)
                continue;
           
            dist = ( dist / circle.fRadius )-1;
            dist *= -1;

            node->m_vVertexList[ivtx].Pos.y += dist* g_fSecondPerFrame * m_HeightBrushstrength;
            newHeight = node->m_vVertexList[ivtx].Pos.y;
            if (newHeight > maxHeight)
            {
                maxHeight = newHeight;
                node->m_Box.m_height = maxHeight;
            }
               
        }

    }
    else if (m_BrushType == BrushType::DOWNBRUSH)
    {
        float newHeight = 0.0f;
        float maxHeight = 0.0f;
        for (int ivtx = 0; ivtx < node->m_vVertexList.size(); ++ivtx)
        {
            TVector2 center(circle.vCenter.x, circle.vCenter.y);
            TVector2 pt(node->m_vVertexList[ivtx].Pos.x, node->m_vVertexList[ivtx].Pos.z);
            float dist = (center - pt).Length();

            if (circle.fRadius <= dist)
                continue;

            dist = (dist / circle.fRadius) - 1;
            dist *= -1;

            node->m_vVertexList[ivtx].Pos.y -= dist * g_fSecondPerFrame * m_HeightBrushstrength;
            newHeight = node->m_vVertexList[ivtx].Pos.y;
            if (newHeight > maxHeight)
            {
                maxHeight = newHeight;
                node->m_Box.m_height = maxHeight;
            }
        }
    }
    else if (m_BrushType == BrushType::SMOOTHBRUSH)
    {
       
        float ave = 0.0f;
        float count = 0.f;

        for (int inode = 0; inode < node->m_vNeghborList.size(); ++inode)
        {
            if (node->m_vNeghborList[inode] == nullptr)continue;
            for (int ivtx = 0; ivtx < node->m_vNeghborList[inode]->m_vVertexList.size(); ++ivtx)
            {
                TVector2 center(circle.vCenter.x, circle.vCenter.y);
                TVector2 pt(node->m_vVertexList[ivtx].Pos.x, node->m_vVertexList[ivtx].Pos.z);
                float dist = (center - pt).Length();

                if (circle.fRadius >= dist)
                    continue;

                ave += node->m_vNeghborList[inode]->m_vVertexList[ivtx].Pos.y;
                count++;
            }

        }
        ave /= count;

        for (int inode = 0; inode < node->m_vNeghborList.size(); ++inode)
        {
            if (node->m_vNeghborList[inode] == nullptr)continue;
            float newHeight = 0.0f;
            float maxHeight = 0.0f;
            for (int ivtx = 0; ivtx < node->m_vNeghborList[inode]->m_vVertexList.size(); ++ivtx)
            {
                TVector2 center(circle.vCenter.x, circle.vCenter.y);
                TVector2 pt(node->m_vVertexList[ivtx].Pos.x, node->m_vVertexList[ivtx].Pos.z);
                float dist = (center - pt).Length();

                if (circle.fRadius >= dist)
                    continue;


                node->m_vNeghborList[inode]->m_vVertexList[ivtx].Pos.y = ave;
                
            }
           
        }
        node->m_Box.m_height = node->m_vVertexList[0].Pos.y;
    }
    else
    {
        //todo
        for (int ivtx = 0; ivtx < node->m_vVertexList.size(); ++ivtx)
        {
            int idx = node->m_vVertexIndexList[ivtx];
            m_ChangeAlphaList.insert(idx);
        }

    }


}
void CQuadTree::UpdateVertexAlpha(Circle circle)
{
    for (auto idx : m_ChangeAlphaList)
    {
        TVector2 center(circle.vCenter.x, circle.vCenter.y);
        TVector2 pt(m_vVertexList[idx].Pos.x, m_vVertexList[idx].Pos.z);
        float dist = (center - pt).Length();

        if (circle.fRadius <= dist)
            continue;

        dist = (dist / circle.fRadius) - 1;
        dist *= -1;


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
    UINT const DataSize = sizeof(BYTE) * 4;
    UINT const RowPitch = DataSize * m_Width;
    CoreInterface::g_pImmediateContext->UpdateSubresource(m_pMap->m_pRoughnessLookUpTex.Get(), 0, NULL, m_pMap->m_fLookup, RowPitch, 0);
    m_ChangeAlphaList.clear();
}
shared_ptr<MapObject> CQuadTree::SetUpMapObject(shared_ptr<CFbxObj> obj, TVector3 scale, TVector3 Rotation, TVector3 Trasnfrom)
{
    
    auto newobj = std::make_shared<MapObject>();
    newobj->SetMapObj(obj.get(), obj->m_AnimInfo);
    newobj->m_Scale._11 = scale.x;
    newobj->m_Scale._22 = scale.y;
    newobj->m_Scale._33 = scale.z;
    newobj->m_Rotation = Rotation;
    newobj->m_Transform._41 = Trasnfrom.x;
    newobj->m_Transform._42 = Trasnfrom.y;
    newobj->m_Transform._43 = Trasnfrom.z;
  
    newobj->m_SRTMat = newobj->m_Scale;
    TMatrix RotateX;
    TMatrix RotateY;
    TMatrix RotateZ;
    TMatrix Rotate;
    D3DXMatrixRotationX(&RotateX, DegreeTORadian(newobj->m_Rotation.x));
    D3DXMatrixRotationY(&RotateY, DegreeTORadian(newobj->m_Rotation.y));
    D3DXMatrixRotationZ(&RotateZ, DegreeTORadian(newobj->m_Rotation.z));
    Rotate = RotateX * RotateY * RotateZ;
    newobj->m_SRTMat *= Rotate;
    newobj->m_SRTMat *= newobj->m_Transform;

    newobj->m_MapObjName = obj->m_csName;
    CheckDuplicationName(newobj->m_MapObjName);
    m_MapObjList.insert(make_pair(newobj->m_MapObjName, newobj));
    return newobj;

}
void CQuadTree::CheckDuplicationName(wstring& name)
{
    auto obj = m_MapObjList.find(name);

    if (obj != m_MapObjList.end())
    {
        int index = 0;
        for (obj = m_MapObjList.begin(); obj != m_MapObjList.end();)
        {
            std::wstring mapname = obj->first;
            size_t nPos = mapname.find(L"#");
            if (nPos != std::wstring::npos) { // 찾고자 하는 문자열이 있었는지 검사
                mapname.erase(nPos);
            }
            if (mapname == name)
            {
                index++;
            }
            obj++;
        }
        name += L"#";
        name += std::to_wstring(index);
    }

}
void CQuadTree::PreOrder(CNode* pNode, CPlane* plane)
{
    if (pNode == nullptr)
    {
        return;
    }
   
    for (int i = 0; i < pNode->m_pChildList.size(); ++i)
    {
        PreOrder(pNode->m_pChildList[i], plane);
    }
    if (pNode->m_bIsLeaf)
    {

        if (CoreInterface::g_pMainCamera->ClassifyOBB(&pNode->m_Box))
        {
            DrawNodeList.push_back(pNode);
        }


    }
   

}
void CQuadTree::PostOrder(CNode* pNode)
{



    if (pNode == nullptr)
    {
        return;
    }

    for (int i = 0; i < pNode->m_pChildList.size(); ++i)
    {
        PostOrder(pNode->m_pChildList[i]);
    }

 

}
void CQuadTree::LevelOrder(CNode* pNode)
{
    if (pNode == nullptr)
    {
        return;
    }


    for (int i = 0; i < pNode->m_pChildList.size(); ++i)
    {
        m_pQueue.push(pNode->m_pChildList[i]);
    }

    if (!m_pQueue.empty())
    {
        CNode* Node = m_pQueue.front();
        m_pQueue.pop();
        LevelOrder(Node);
    }





}
CNode* CQuadTree::GetRootNode()
{
    return m_pRootNode;
}
TVector2 CQuadTree::GetHeightFromNode(DWORD dwTL, DWORD dwTR, DWORD dwBL, DWORD dwBR)
{

    DWORD dwStartRow = dwTL / m_pMap->m_iCol;
    DWORD dwEndRow = dwBL / m_pMap->m_iCol;

    DWORD dwStartCol = dwTL % m_pMap->m_iCol;
    DWORD dwEndCol = dwTR % m_pMap->m_iCol;

    TVector2 vHeight;
    vHeight.x = -999999.0f;
    vHeight.y = 999999.0f;

    // 0,  4, 
    // 20 ,24

    for (DWORD dwRow = dwStartRow; dwRow < dwEndRow; dwRow++)
    {
        for (DWORD dwCol = dwStartCol; dwCol < dwEndCol; dwCol++)
        {
            if (m_pMap->m_vVertexList[dwRow * m_pMap->m_iCol + dwCol].Pos.y > vHeight.x)
            {
                vHeight.x = m_pMap->m_vVertexList[dwRow * m_pMap->m_iCol + dwCol].Pos.y;
            }
            if (m_pMap->m_vVertexList[dwRow * m_pMap->m_iCol + dwCol].Pos.y < vHeight.y)
            {
                vHeight.y = m_pMap->m_vVertexList[dwRow * m_pMap->m_iCol + dwCol].Pos.y;
            }
        }
    }
    return vHeight;
}
TVector2 CQuadTree::GetHeightFromNode(CNode* pNode)
{
    DWORD dwTL = pNode->m_CornerIndex[0];
    DWORD dwTR = pNode->m_CornerIndex[1];
    DWORD dwBR = pNode->m_CornerIndex[2];
    DWORD dwBL = pNode->m_CornerIndex[3];

    DWORD dwStartCol = dwTL % m_pMap->m_iCol;
    DWORD dwEndCol = dwTR % m_pMap->m_iCol;
    DWORD dwStartRow = dwTL / m_pMap->m_iCol;
    DWORD dwEndRow = dwBL / m_pMap->m_iCol;

    TVector2 vHeight;
    vHeight.x = -99999999.0f;
    vHeight.y = 99999999.0f;

    for (int dwRow = dwStartRow; dwRow < dwEndRow; dwRow++)
    {
        for (int dwCol = dwStartCol; dwCol < dwEndCol; dwCol++)
        {
            DWORD dwCurrent = dwRow * m_pMap->m_iCol + dwCol;
            if (m_pMap->m_vVertexList[dwCurrent].Pos.y > vHeight.x)
            {
                vHeight.x = m_pMap->m_vVertexList[dwCurrent].Pos.y;
            }
            if (m_pMap->m_vVertexList[dwCurrent].Pos.y < vHeight.y)
            {
                vHeight.y = m_pMap->m_vVertexList[dwCurrent].Pos.y;
            }
        }
    }
    return vHeight;

}
void CQuadTree::CreateBoundingBox(CNode* pNode)
{
    DWORD dwV0 = pNode->m_CornerIndex[0];
    DWORD dwV1 = pNode->m_CornerIndex[3];

    //TVector2 vHeight = GetHeightFromNode(pNode->m_CornerIndex[0], pNode->m_CornerIndex[1],
    //    pNode->m_CornerIndex[2], pNode->m_CornerIndex[3]);
    TVector2 vHeight = GetHeightFromNode(pNode);
    pNode->m_Box.m_Max.x = m_pMap->m_vVertexList[dwV1].Pos.x;
    pNode->m_Box.m_Max.z = m_pMap->m_vVertexList[dwV0].Pos.z;
    pNode->m_Box.m_Min.x = m_pMap->m_vVertexList[dwV0].Pos.x;
    pNode->m_Box.m_Min.z = m_pMap->m_vVertexList[dwV1].Pos.z;


    pNode->m_Box.m_Max.y = vHeight.x;
    pNode->m_Box.m_Min.y = vHeight.y;

    pNode->m_Box.m_vAxis[0] = { 1,0,0 };
    pNode->m_Box.m_vAxis[1] = { 0,1,0 };
    pNode->m_Box.m_vAxis[2] = { 0,0,1 };

    pNode->m_Box.m_pCenter = (pNode->m_Box.m_Max + pNode->m_Box.m_Min) * 0.5f;
    pNode->m_Box.m_fExtent[0] = pNode->m_Box.m_Max.x - pNode->m_Box.m_pCenter.x;
    pNode->m_Box.m_fExtent[1] = pNode->m_Box.m_Max.y - pNode->m_Box.m_pCenter.y;
    pNode->m_Box.m_fExtent[2] = pNode->m_Box.m_Max.z - pNode->m_Box.m_pCenter.z;

    pNode->m_Box.m_Width = pNode->m_Box.m_Max.x - pNode->m_Box.m_Min.x;
    pNode->m_Box.m_height = pNode->m_Box.m_Max.y - pNode->m_Box.m_Min.y;
    pNode->m_Box.m_Depth = pNode->m_Box.m_Max.z - pNode->m_Box.m_Min.z;
    //todo
    pNode->m_Sphere.vCenter = pNode->m_Box.m_pCenter;
    pNode->m_Sphere.fRadius = fabs(pNode->m_Box.m_height);

    
    // create box 

  
}
CNode* CQuadTree::CreateNode(CNode* pParent,DWORD LT ,DWORD RT , DWORD LB,DWORD RB)
{
    CNode* NewNode = new CNode(pParent, LT, RT, LB, RB);

    NewNode->m_pChildList.reserve(4);
    NewNode->m_pCornerList.reserve(4);

    
    int iFace = (RT - LT) * (RT - LT) * 2;
    NewNode->m_vIndexList.resize(iFace * 3);

    int CurIndex = 0;

    int size = m_vVertexList[RT].Pos.x - m_vVertexList[LT].Pos.x;

    for (int iRow = 0; iRow < size; iRow++)
    {
        for (int iCol = 0; iCol < size; ++iCol)
        {
            int NextRow = iRow + 1;
            int NextCol = iCol + 1;
           NewNode->m_vIndexList[CurIndex + 0] = iRow * (size+1) + iCol;
           NewNode->m_vIndexList[CurIndex + 1] = iRow * (size + 1) + NextCol;
           NewNode->m_vIndexList[CurIndex + 2] = NextRow * (size + 1) + iCol;
           
           NewNode->m_vIndexList[CurIndex + 3] = NewNode->m_vIndexList[CurIndex + 2];
           NewNode->m_vIndexList[CurIndex + 4] = NewNode->m_vIndexList[CurIndex + 1];
           NewNode->m_vIndexList[CurIndex + 5] = NextRow * (size + 1) + NextCol;
          
            CurIndex += 6;
        }
       
    }

    if (pParent)
    {
        NewNode->m_iDepth = pParent->m_iDepth + 1;
        if ((DWORD)m_iMaxDepth < NewNode->m_iDepth)
        {
            m_iMaxDepth = NewNode->m_iDepth;
        }
    }

    ldiv_t divValue = ldiv(LT, m_Width);
    NewNode->m_dwPositionIndex[0] = divValue.rem / (RT - LT);
    NewNode->m_dwPositionIndex[1] = divValue.quot / (RT - LT);





   // NewNode->CreateIndexBuffer();
    CreateBoundingBox(NewNode);
    UpdateVertexList(NewNode);
    NewNode->CreateVertexBuffer();
    //m_pLeafNodeList.push_back(NewNode);

 
 
   return NewNode;

}
bool CQuadTree::UpdateVertexList(CNode* pNode)
{

    int iNumCols = m_pMap->m_iCol;
    int iStartRow = pNode->m_CornerIndex[0] / iNumCols;
    int iEndRow = pNode->m_CornerIndex[2] / iNumCols;
    int iStartCol = pNode->m_CornerIndex[0] % iNumCols;
    int iEndCol = pNode->m_CornerIndex[1] % iNumCols;

    int iNumColCell = iEndCol - iStartCol;
    int iNumRowCell = iEndRow - iStartRow;
    pNode->m_vVertexList.resize((iEndCol - iStartCol + 1) * (iEndRow - iStartRow + 1));

    int iIndex = 0;
    for (int iRow = iStartRow; iRow <= iEndRow; iRow++)
    {
        for (int iCol = iStartCol; iCol <= iEndCol; iCol++)
        {
            int iCurrentIndex = iRow * iNumCols + iCol;
            pNode->m_vVertexList[iIndex++] = m_pMap->m_vVertexList[iCurrentIndex];
            pNode->m_vVertexIndexList.push_back(iCurrentIndex);
        }
    }
    if (pNode->m_vVertexList.size() > 0) return true;
    return false;

}
bool CQuadTree::UpdateMapVertexList()
{
    for (auto drawNode : DrawNodeList)
    {
        CNode* pNode = drawNode;

        int iNumCols = m_pMap->m_iCol;
        int iStartRow = pNode->m_CornerIndex[0] / iNumCols;
        int iEndRow = pNode->m_CornerIndex[2] / iNumCols;
        int iStartCol = pNode->m_CornerIndex[0] % iNumCols;
        int iEndCol = pNode->m_CornerIndex[1] % iNumCols;
        int iNumColCell = iEndCol - iStartCol;
        int iNumRowCell = iEndRow - iStartRow;
        int iIndex = 0;
        for (int iRow = iStartRow; iRow <= iEndRow; iRow++)
        {
            for (int iCol = iStartCol; iCol <= iEndCol; iCol++)
            {
                int iCurrentIndex = iRow * iNumCols + iCol;
                m_pMap->m_vVertexList[iCurrentIndex] = pNode->m_vVertexList[iIndex++];
            }
        }
    }
    return false;
}
void CQuadTree::DebugBox(CBox box, CSphere sphere)
{
    //shared_ptr<CObject> TestBox = make_shared<CObject>();
    //TestBox->m_vVertexList.resize(24);
    //float w2 = box.m_Max.x;
    //float h2 = box.m_Max.y;
    //float d2 = box.m_Max.z;

    //float w3 = box.m_Min.x;
    //float h3 = box.m_Min.y;
    //float d3 = box.m_Min.z;

    //// 앞면
    //TestBox->m_vVertexList[0] = PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[1] = PNCT_VERTEX{ TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[2] = PNCT_VERTEX{ TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[3] = PNCT_VERTEX{ TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //// 뒷면 ->                                                         TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[4] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[5] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[6] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[7] = PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //// 윗면 ->
    //TestBox->m_vVertexList[8] = PNCT_VERTEX{ TVector3(w3, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[9] = PNCT_VERTEX{ TVector3(w3, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[10] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[11] = PNCT_VERTEX{ TVector3(+w2, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //// 아랫- >
    //TestBox->m_vVertexList[12] = PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[13] = PNCT_VERTEX{ TVector3(+w2, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[14] = PNCT_VERTEX{ TVector3(+w2, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[15] = PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //// 왼쪽- >
    //TestBox->m_vVertexList[16] =PNCT_VERTEX{ TVector3(w3, h3, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[17] =PNCT_VERTEX{ TVector3(w3, +h2, +d2), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[18] =PNCT_VERTEX{ TVector3(w3, +h2, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[19] =PNCT_VERTEX{ TVector3(w3, h3, d3), TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //// 오른->면																
    //TestBox->m_vVertexList[20] = PNCT_VERTEX{ TVector3(+w2, h3, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[21] = PNCT_VERTEX{ TVector3(+w2, +h2, d3),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[22] = PNCT_VERTEX{ TVector3(+w2, +h2, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vVertexList[23] = PNCT_VERTEX{ TVector3(+w2, h3, +d2),TVector3(0.0f, 1.0f,0.0f) ,TVector4(0.0f,0.0f,1.0f,1.0f),TVector2(0.0f,0.0f) };
    //TestBox->m_vIndexList.resize(36);
    //
    //// 앞면
    //TestBox->m_vIndexList[0] = 0; TestBox->m_vIndexList[1] = 1; TestBox->m_vIndexList[2] = 2;
    //TestBox->m_vIndexList[3] = 0; TestBox->m_vIndexList[4] = 2; TestBox->m_vIndexList[5] = 3;
    //// 뒷면
    //TestBox->m_vIndexList[6] = 4; TestBox->m_vIndexList[7] = 5; TestBox->m_vIndexList[8] = 6;
    //TestBox->m_vIndexList[9] = 4; TestBox->m_vIndexList[10] = 6; TestBox->m_vIndexList[11] = 7;
    //// 윗면
    //TestBox->m_vIndexList[12] = 8; TestBox->m_vIndexList[13] = 9; TestBox->m_vIndexList[14] = 10;
    //TestBox->m_vIndexList[15] = 8; TestBox->m_vIndexList[16] = 10; TestBox->m_vIndexList[17] = 11;
    //// 아랫면
    //TestBox->m_vIndexList[18] = 12; TestBox->m_vIndexList[19] = 13; TestBox->m_vIndexList[20] = 14;
    //TestBox->m_vIndexList[21] = 12; TestBox->m_vIndexList[22] = 14; TestBox->m_vIndexList[23] = 15;
    //// 왼쪽면
    //TestBox->m_vIndexList[24] = 16; TestBox->m_vIndexList[25] = 17; TestBox->m_vIndexList[26] = 18;
    //TestBox->m_vIndexList[27] = 16; TestBox->m_vIndexList[28] = 18; TestBox->m_vIndexList[29] = 19;
    //// 오른쪽면
    //TestBox->m_vIndexList[30] = 20; TestBox->m_vIndexList[31] = 21; TestBox->m_vIndexList[32] = 22;
    //TestBox->m_vIndexList[33] = 20; TestBox->m_vIndexList[34] = 22; TestBox->m_vIndexList[35] = 23;

    /*
    PNCT_VERTEX v;
    float radius = sphere.fRadius; // 구의 반지름
    int stackCount = 10; // 가로 분할
    int sliceCount = 10; // 세로 분할
    // 북극
    v.Pos = TVector3(sphere.vCenter.x, sphere.vCenter.y, sphere.vCenter.z);
    v.Tex = TVector2(0.5f, 0.0f);
    TestBox->m_vVertexList.push_back(v);

    float stackAngle = XM_PI / stackCount;
    float sliceAngle = XM_2PI / sliceCount;

    float deltaU = 1.f / static_cast<float>(sliceCount);
    float deltaV = 1.f / static_cast<float>(stackCount);

    // 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
    for (int y = 1; y <= stackCount - 1; ++y)
    {
        float phi = y * stackAngle;

        // 고리에 위치한 정점
        for (int x = 0; x <= sliceCount; ++x)
        {
            float theta = x * sliceAngle;

            v.Pos.x = sphere.vCenter.x+radius * sinf(phi) * cosf(theta);
            v.Pos.y = sphere.vCenter.y+radius * cosf(phi);
            v.Pos.z = sphere.vCenter.z+radius * sinf(phi) * sinf(theta);

            v.Tex = TVector2(deltaU * x, deltaV * y);
            v.Nor = v.Pos;
            v.Nor.Normalize();


            TestBox->m_vVertexList.push_back(v);
        }
    }

    // 남극
    v.Pos = TVector3(sphere.vCenter.x, -sphere.vCenter.y, sphere.vCenter.z);
    v.Nor = v.Nor;
    v.Nor.Normalize();
    TestBox->m_vVertexList.push_back(v);



    // 북극 인덱스
    for (int i = 0; i <= sliceCount; ++i)
    {
        //  [0]
        //   |  \
   		//  [i+1]-[i+2]
        TestBox->m_vIndexList.push_back(0);
        TestBox->m_vIndexList.push_back(i + 2);
        TestBox->m_vIndexList.push_back(i + 1);
    }

    // 몸통 인덱스
    int ringVertexCount = sliceCount + 1;
    for (int y = 0; y < stackCount - 2; ++y)
    {
        for (int x = 0; x < sliceCount; ++x)
        {
            //  [y, x]-[y, x+1]
            //  |		/
            //  [y+1, x]
            TestBox->m_vIndexList.push_back(1 + (y)*ringVertexCount + (x));
            TestBox->m_vIndexList.push_back(1 + (y)*ringVertexCount + (x + 1));
            TestBox->m_vIndexList.push_back(1 + (y + 1) * ringVertexCount + (x));
            //		 [y, x+1]
            //		 /	  |
            //  [y+1, x]-[y+1, x+1]
            TestBox->m_vIndexList.push_back(1 + (y + 1) * ringVertexCount + (x));
            TestBox->m_vIndexList.push_back(1 + (y)*ringVertexCount + (x + 1));
            TestBox->m_vIndexList.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
        }
    }

    // 남극 인덱스
    int bottomIndex = static_cast<int>(TestBox->m_vVertexList.size()) - 1;
    int lastRingStartIndex = bottomIndex - ringVertexCount;
    for (int i = 0; i < sliceCount; ++i)
    {
        //  [last+i]-[last+i+1]
        //  |      /
        //  [bottom]
        TestBox->m_vIndexList.push_back(bottomIndex);
        TestBox->m_vIndexList.push_back(lastRingStartIndex + i);
        TestBox->m_vIndexList.push_back(lastRingStartIndex + i + 1);
    }
    */

   //TestBox->Create(L"", L"../../Resource/Plane.hlsl");
   //
   //TestBoxlist.push_back(TestBox);
}
bool CQuadTree::Init()
{
    if (m_pTexSRV[0] == nullptr)
    {
        CTexture* tex0 = new CTexture;
        tex0->SetSRV(m_pMap->m_pLookupRV.Get());
        tex0->m_csName = L"AlphaMap.png";
        m_pTexSRV[0]=(tex0->GetSRV());
        CTexture* tex1 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(L"../../Resource/000.jpg"));
        m_pTexSRV[1]=(tex1->GetSRV());
        CTexture* tex2 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(L"../../Resource/008.jpg"));
        m_pTexSRV[2]=(tex2->GetSRV());
        CTexture* tex3 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(L"../../Resource/019.bmp"));
        m_pTexSRV[3]=((tex3->GetSRV()));
        CTexture* tex4 = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(L"../../Resource/Tile60.jpg"));
        m_pTexSRV[4]=(tex4->GetSRV());
        m_pTextureList.push_back(tex0);
        m_pTextureList.push_back(tex1);
        m_pTextureList.push_back(tex2);
        m_pTextureList.push_back(tex3);
        m_pTextureList.push_back(tex4);

        CTexture* normal = const_cast<CTexture*>(CTextureMgr::GetInstance().Load(L"../../Resource/stone_wall_normal_map.bmp"));
        m_pNormalMapSRV[0] = normal->GetSRV();
        m_pNormalMapList.push_back(normal);
    }

    return true;
}
bool CQuadTree::PreFrame()
{
    return true;
}
bool CQuadTree::Frame(Select* select)
{

    DrawNodeList.clear();
    PreOrder(m_pRootNode, CoreInterface::g_pMainCamera->m_Plane);
    GetDrawPatchNode(DrawNodeList);

    m_iNumFace = 0;
    for (auto node : m_vDrawPatchNodeList)
    {
        GetLODType(node);
    }


    Circle cir;
    if (m_BrushType <= BrushType::LAYER4)
    {
        for (auto node : DrawNodeList)
        {
            if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_HOLD && select->GetIntersectionBox(&node->m_Box))
            {
                cir.vCenter = TVector2(select->m_vIntersection.x, select->m_vIntersection.z);
                cir.fRadius = select->m_fPickDistance;
                for (auto node2 : DrawNodeList)
                {
                    if (CCollision::CircleToBox(cir, node2->m_Box))
                    {
                        select->m_vSelectNodeList.insert(node2);
                        FindNeighborVertex(node2, cir);
                        //m_Select.InterpolationNodeHeight();
                        CoreInterface::g_pImmediateContext->UpdateSubresource(node2->m_pVertexBuffer.Get(),
                            0, NULL, &node2->m_vVertexList.at(0), 0, 0);
                    }
                }
                UpdateVertexAlpha(cir);
                UpdateMapVertexList();
                break;
            }
        }
  


    }
    if (CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_UP&& m_BrushType <= BrushType::SMOOTHBRUSH)
    {
        if (!select->m_vSelectNodeList.empty())
        {
            for (auto node : select->m_vSelectNodeList)
            {
                UpdateNodeBoundingBox(node, node->m_Box.m_height);
                //m_pMap->CalcPerVertexNormalsFastLookup();
            }
            select->m_vSelectNodeList.clear();
        }

    }
    


    return true;
}
bool CQuadTree::PostFrame()
{
    return true;
}
bool CQuadTree::Render(CPlane* plane)
{
   
    m_pMap->PreRender();
    UINT Stride = sizeof(TVector3);
    UINT offset = 0;
    CoreInterface::g_pImmediateContext->IASetVertexBuffers(1, 1, m_pMap->m_pTangentVB.GetAddressOf(), &Stride, &offset);//p 159참조


    CoreInterface::g_pImmediateContext->PSSetShaderResources(1, 1,m_pTexSRV[0].GetAddressOf());
    CoreInterface::g_pImmediateContext->PSSetShaderResources(2, 4, m_pTexSRV[1].GetAddressOf());
    CoreInterface::g_pImmediateContext->PSSetShaderResources(6, 4, m_pNormalMapSRV[0].GetAddressOf());

    for (auto node : m_vDrawPatchNodeList)
    {
        UINT Stride = sizeof(PNCT_VERTEX);
        UINT offset = 0;

        CoreInterface::g_pImmediateContext->IASetVertexBuffers(0, 1, node->m_pVertexBuffer.GetAddressOf(), &Stride, &offset);//p 159참조
        CoreInterface::g_pImmediateContext->IASetIndexBuffer(m_vLodIndexList[node->m_dwLodLevel]->IndexBufferList[node->m_dwLodType].Get(), DXGI_FORMAT_R32_UINT, 0);
        CoreInterface::g_pImmediateContext->DrawIndexed(m_vLodIndexList[node->m_dwLodLevel]->IndexList[node->m_dwLodType].size(), 0, 0);
      
    
    }


    return true;
}
bool CQuadTree::Release()
{
    if (m_pRootNode)m_pRootNode->Release();
    if (m_pMap)m_pMap->Release();
    

    m_vVertexList.clear();
    m_vTreeIndexList.clear();
    DrawNodeList.clear();
    TestBoxlist.clear();
    m_ChangeAlphaList.clear();
    m_vLeafNodeList.clear();
    m_MapObjList.clear();
   

    return true;
}
void CQuadTree::BuildQuadTree(std::shared_ptr<CMap> pMap, DWORD Width, DWORD Height, std::vector<PNCT_VERTEX> vVertexList)
{

    m_bUsedIndexList = true;
    m_Width = Width;
    m_Height = Height;

    m_pMap = pMap;
    m_vVertexList.reserve(vVertexList.size());
    m_vVertexList = vVertexList;
    m_pRootNode = CreateNode(nullptr, 0, Width - 1, Width * (Height - 1), Width * Height - 1);
    m_pRootNode->m_iCellRow = Height;
    m_pRootNode->m_iCellCol = Width;

    InitLOD(m_pRootNode, m_iMaxDepthLimit);

    BuildTree(m_pRootNode);
    m_pRootNode->CreateIndexBuffer();
    m_vLeafNodeList[0]->CreateIndexBuffer();

    for (auto node : m_vLeafNodeList)
    {
        node->m_pIndexBuffer = m_vLeafNodeList[0]->m_pIndexBuffer;
    }

    for (int iNode = 0; iNode < m_pRootNode->m_pChildList.size(); ++iNode)
    {
        SetNeighborNode(m_pRootNode->m_pChildList[iNode]);
    }
    SetLOD(m_Width,m_iMaxDepth);
    ComputeStaticLodIndex(m_iNumCell);
    
    Init();



}
bool CQuadTree::SubDivide(CNode* pNode)
{
    if (pNode == NULL) return false;



    // 현재 노드의 실제 크기를 계산한다.
    DWORD dwOffsetX = (pNode->m_CornerIndex[1] - pNode->m_CornerIndex[0]);
    DWORD dwOffsetZ = ((pNode->m_CornerIndex[2] - pNode->m_CornerIndex[0]) / m_Width);

    DWORD dwWidthSplit = CheckSize(dwOffsetX);
    DWORD dwHeightplit = CheckSize(dwOffsetZ);

    // 최대 깊이 한도로 제한한다.
    if ((DWORD)m_iMaxDepthLimit <= pNode->m_iDepth)
    {
        pNode->m_bIsLeaf = TRUE;
        m_vLeafNodeList.push_back(pNode);
            DebugBox(pNode->m_Box, pNode->m_Sphere);
        //CreateVertexList(pNode);
        return false;
    }
    // 4등분 할수 없으면 더이상 분할하지 않는다.
    if (dwWidthSplit < (DWORD)m_iMinSplitDistance || dwHeightplit < (DWORD)m_iMinSplitDistance)
    {
        pNode->m_bIsLeaf = TRUE;
        m_vLeafNodeList.push_back(pNode);
            DebugBox(pNode->m_Box, pNode->m_Sphere);
        //CreateVertexList(pNode);
        return false;
    }
    // 자식 노드가 지정된 분할크기보다 작다면 더이상 분할하지 않는다.
    if (dwWidthSplit < m_fMinDivideSize || dwHeightplit < m_fMinDivideSize)
    {
        pNode->m_bIsLeaf = TRUE;
        m_vLeafNodeList.push_back(pNode);
            DebugBox(pNode->m_Box, pNode->m_Sphere);
       //CreateVertexList(pNode);
        return false;
    }

    // 우상귀 = 좌측코너 + 스플릿크기
    // 우하귀 = 우상귀 + ( 스플릿 크기 * 전체맵 가로크기 )
    DWORD dwSplitCenter = (pNode->m_CornerIndex[0] + dwWidthSplit) + (dwHeightplit * m_Width);

    DWORD dwEdgeCenter[4];
    dwEdgeCenter[0] = pNode->m_CornerIndex[0] + dwWidthSplit;
    dwEdgeCenter[1] = pNode->m_CornerIndex[1] + dwHeightplit * m_Width;
    dwEdgeCenter[2] = pNode->m_CornerIndex[2] + dwWidthSplit;
    dwEdgeCenter[3] = pNode->m_CornerIndex[0] + dwHeightplit * m_Width;

    pNode->m_pChildList.push_back(CreateNode(pNode, pNode->m_CornerIndex[0],
        dwEdgeCenter[0],
        dwEdgeCenter[3],
        dwSplitCenter));

    pNode->m_pChildList.push_back(CreateNode(pNode, dwEdgeCenter[0],
        pNode->m_CornerIndex[1],
        dwSplitCenter,
        dwEdgeCenter[1]));

    pNode->m_pChildList.push_back(CreateNode(pNode, dwEdgeCenter[3],
        dwSplitCenter,
        pNode->m_CornerIndex[2],
        dwEdgeCenter[2]));

    pNode->m_pChildList.push_back(CreateNode(pNode, dwSplitCenter,
        dwEdgeCenter[1],
        dwEdgeCenter[2],
        pNode->m_CornerIndex[3]));
    return true;
}
DWORD CQuadTree::CheckSize(DWORD dwSize)
{
    float fCount = 0;
    DWORD dwChecker = dwSize / 2;
    if (dwChecker <= 0) return 0;

    while (dwChecker != 0)
    {
        dwChecker >>= 1;
        fCount += 1.0f;
    }
    float fMinimum = pow(2.0f, fCount - 1.0f);
    float fMaximum = pow(2.0f, fCount);
    // fMin = abs( fMin - P0 ) - abs( P1 - fMin );
    // fMax = abs( fMax - P0 ) - abs( P1 - fMax );
    // return min( fMin, fMax );
    float fMin = fabs(fMinimum - fabs(((float)dwSize - fMinimum)));
    float fMax = fabs(fMaximum - fabs(((float)dwSize - fMaximum)));

    // 비교값이 같다면 작은 분할크기를 반환한다. 예) fMin=1 : fMax=1 => fMinimum=2 : fMaximum=4
    if (fMin <= fMax)
    {
        return (DWORD)fMinimum;
    }
    return (DWORD)fMaximum;
}

