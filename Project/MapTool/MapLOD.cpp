#include "MapLOD.h"
bool MapLOD::CreateIndexBuffer(std::shared_ptr<StaticData> data)
{
    data->IndexBufferList.resize(data->IndexList.size());
    for (int iData = 0; iData < data->IndexList.size(); ++iData)
    {
        if (data->IndexList.size() <= 0)break;

        D3D11_BUFFER_DESC Desc;
        Desc.ByteWidth = sizeof(DWORD) * data->IndexList[iData].size();//버퍼의 크기
        Desc.Usage = D3D11_USAGE_DEFAULT;//버퍼를 어디에서 읽을것인지
        Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//버퍼의 사용용도
        Desc.CPUAccessFlags = 0;//cpu의 엑세스유형 필요없으면 null
        Desc.MiscFlags = 0;//추가적인 옵션
        Desc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA pInitialData;
        pInitialData.pSysMem = &data->IndexList[iData].at(0);

        CoreInterface::g_pDevice->CreateBuffer(&Desc, &pInitialData, data->IndexBufferList[iData].GetAddressOf());


    }


    return true;
}
DWORD MapLOD::GetIndexCounter(DWORD dwData, DWORD dwNumCell)
{
    DWORD dwNumFace = (dwNumCell * dwNumCell * 4);
    DWORD dwLevelCount = 0;
    if (dwData & 8) ++dwLevelCount;
    if (dwData & 4) ++dwLevelCount;
    if (dwData & 2) ++dwLevelCount;
    if (dwData & 1) ++dwLevelCount;
    dwNumFace = (dwNumFace + (dwNumCell * dwLevelCount));


    return dwNumFace * 3;
}

void MapLOD::GetDrawPatchNode(vector<CNode*> drawNodeList)
{
    m_vDrawPatchNodeList.clear();
    for (auto node : drawNodeList)
    {
        AddDrawPatchNode(node);
    }
}
void MapLOD::AddDrawPatchNode(CNode* pNode)
{
    if (pNode->m_bIsLeaf)
    {
        m_vDrawPatchNodeList.push_back(pNode);

        GetLodSubIndex(pNode);
        return;

    }
    for (auto node : pNode->m_pChildList)
    {
        AddDrawPatchNode(node);
    }


}
DWORD MapLOD::GetLodSubIndex(CNode* pNode)
{
    float fRatio = GetExpansionRatio(pNode->m_Box.m_pCenter);
    pNode->m_dwLodLevel = fRatio * m_iPatchLodCount;

    return pNode->m_dwLodLevel;
}
float MapLOD::GetExpansionRatio(TVector3 pCenter)
{
    TVector3 len = CoreInterface::g_pMainCamera->m_vCameraPos - pCenter;
    float fDist = D3DXVec3Length(&len);
    if (fDist > CoreInterface::g_pMainCamera->m_fFar)
    {
        return 1.0f;
    }
    float fRatio = fDist / CoreInterface::g_pMainCamera->m_fFar;
    return fRatio;
}
DWORD MapLOD::GetLODType(CNode* pNode)
{
    if (pNode->m_vNeghborList.size() <= 0) return 0;
    DWORD dwType = 0;
    if (pNode->m_vNeghborList[0] && pNode->m_vNeghborList[0]->m_dwLodLevel < pNode->m_dwLodLevel) //상단 체크
        dwType += 1;
    if (pNode->m_vNeghborList[1] && pNode->m_vNeghborList[1]->m_dwLodLevel < pNode->m_dwLodLevel) //하단 체크
        dwType += 4;
    if (pNode->m_vNeghborList[2] && pNode->m_vNeghborList[2]->m_dwLodLevel < pNode->m_dwLodLevel) //좌측 체크
        dwType += 8;
    if (pNode->m_vNeghborList[3] && pNode->m_vNeghborList[3]->m_dwLodLevel < pNode->m_dwLodLevel) //우측 체크
        dwType += 2;

    pNode->m_dwLodType = dwType;

    return dwType;
}
void MapLOD::FineNeighborNode(CNode* node)
{
    node->m_vNeghborList.resize(4);
    DWORD dwNumPatchCount = pow(2.0f, (float)node->m_iDepth);
    DWORD dwNeighborCol, dwNeighborRow;

    if (node->m_dwPositionIndex[1] > 0) //윗 노드가 있다
    {
        dwNeighborCol = node->m_dwPositionIndex[0];
        dwNeighborRow = (node->m_dwPositionIndex[1] - 1) * dwNumPatchCount;
        node->m_vNeghborList[0] = m_LevelList[node->m_iDepth][dwNeighborRow + dwNeighborCol];

    }
    if (node->m_dwPositionIndex[1] < dwNumPatchCount - 1)//아랫 노드가 있다
    {
        dwNeighborCol = node->m_dwPositionIndex[0];
        dwNeighborRow = (node->m_dwPositionIndex[1] + 1) * dwNumPatchCount;
        node->m_vNeghborList[1] = m_LevelList[node->m_iDepth][dwNeighborRow + dwNeighborCol];
    }
    if (node->m_dwPositionIndex[0] > 0)//왼쪽 노드가 있다 
    {
        //todo
        dwNeighborCol = node->m_dwPositionIndex[0] - 1;
        dwNeighborRow = (node->m_dwPositionIndex[1]) * dwNumPatchCount;
        node->m_vNeghborList[2] = m_LevelList[node->m_iDepth][dwNeighborRow + dwNeighborCol];
    }
    if (node->m_dwPositionIndex[0] < dwNumPatchCount - 1)//오른쪽 노드가 있다
    {
        //todo
        dwNeighborCol = node->m_dwPositionIndex[0] + 1;
        dwNeighborRow = (node->m_dwPositionIndex[1]) * dwNumPatchCount;
        node->m_vNeghborList[3] = m_LevelList[node->m_iDepth][dwNeighborRow + dwNeighborCol];
    }




}
void MapLOD::SetLOD(DWORD dwWidth, int iNumDepth)
{

    m_iNumCell = (int)((dwWidth - 1) / pow(2.0f, (float)iNumDepth));
    m_iPatchLodCount = (int)(log((float)m_iNumCell) / log(2.0f));

}
void MapLOD::SetNeighborNode(CNode* node)
{
    FineNeighborNode(node);
    for (int iNode = 0; iNode < node->m_pChildList.size(); ++iNode)
    {

        SetNeighborNode(node->m_pChildList[iNode]);

    }


}
void MapLOD::InitLOD(CNode* rootNode, int maxDepth)
{
    m_LevelList.resize(maxDepth + 1);

    for (int iLevel = 1; iLevel < maxDepth + 1; ++iLevel)
    {
        m_LevelList[iLevel].resize(pow(4.0f, iLevel));
    }

    m_LevelList[0].push_back(m_pRootNode);

}
void MapLOD::ComputeStaticLodIndex(int iMaxCell)
{
    m_vLodIndexList.reserve(m_iPatchLodCount + 1);

    std::shared_ptr<StaticData> pStaticData = make_shared<StaticData>();
    pStaticData->dwLevel = 0;
    pStaticData->IndexList.resize(1);

    for (DWORD dwRow = 0; dwRow < (DWORD)iMaxCell; ++dwRow)
    {
        for (DWORD dwCol = 0; dwCol < (DWORD)iMaxCell; ++dwCol)
        {
            DWORD dwNextRow = dwRow + 1;
            DWORD dwNextCol = dwCol + 1;
            pStaticData->IndexList[0].push_back(dwRow * (iMaxCell + 1) + dwCol);
            pStaticData->IndexList[0].push_back(dwRow * (iMaxCell + 1) + dwNextCol);
            pStaticData->IndexList[0].push_back(dwNextRow * (iMaxCell + 1) + dwCol);
            pStaticData->IndexList[0].push_back(dwNextRow * (iMaxCell + 1) + dwCol);
            pStaticData->IndexList[0].push_back(dwRow * (iMaxCell + 1) + dwNextCol);
            pStaticData->IndexList[0].push_back(dwNextRow * (iMaxCell + 1) + dwNextCol);
        }
    }
    m_vLodIndexList.push_back(pStaticData);
    CreateIndexBuffer(pStaticData);


    for (DWORD dwLevel = 0; dwLevel < m_iPatchLodCount; ++dwLevel)
    {
        DWORD dwOffset = pow(2.0f, (float)dwLevel + 1);
        DWORD dwNumCell = m_iNumCell / dwOffset;

        std::shared_ptr<StaticData> pStaticData = make_shared<StaticData>();
        pStaticData->dwLevel = dwLevel + 1;
        pStaticData->IndexList.resize(16);

        for (int iData = 0; iData < 16; ++iData)
        {
            DWORD dwIndexCounter = GetIndexCounter(iData, dwNumCell);

            pStaticData->IndexList[iData].resize(dwIndexCounter);
            int iIndex = 0;
            for (DWORD dwRow = 0; dwRow < iMaxCell; dwRow += dwOffset)
            {
                for (DWORD dwCol = 0; dwCol < iMaxCell; dwCol += dwOffset)
                {
                    DWORD dwTL = dwRow * (iMaxCell + 1) + dwCol;
                    DWORD dwTR = dwTL + dwOffset;
                    DWORD dwBL = dwOffset * (iMaxCell + 1) + dwTL;
                    DWORD dwBR = dwBL + dwOffset;
                    DWORD dwCP = (dwTL + dwBR) / 2;

                    if (iData != 0 &&
                        (dwRow == 0 ||
                            dwRow == ((dwNumCell - 1) * dwOffset) ||
                            dwCol == 0 ||
                            dwCol == ((dwNumCell - 1) * dwOffset))
                        )
                    {
                        if ((iData & 1) && dwRow == 0)
                        {
                            DWORD dwUpperCenter = (dwTL + dwTR) / 2;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwTL;
                            pStaticData->IndexList[iData][iIndex++] = dwUpperCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwUpperCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwTR;
                        }
                        else
                        {
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwTL;
                            pStaticData->IndexList[iData][iIndex++] = dwTR;
                        }
                        if ((iData & 2) && (dwCol == (dwNumCell - 1) * dwOffset))
                        {
                            DWORD dwRightCenter = (dwTR + dwBR) / 2;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwTR;
                            pStaticData->IndexList[iData][iIndex++] = dwRightCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwRightCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwBR;
                        }
                        else
                        {
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwTR;
                            pStaticData->IndexList[iData][iIndex++] = dwBR;
                        }
                        if ((iData & 4) && (dwRow == (dwNumCell - 1) * dwOffset))
                        {
                            DWORD dwLowerCenter = (dwBR + dwBL) / 2;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwBR;
                            pStaticData->IndexList[iData][iIndex++] = dwLowerCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwLowerCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwBL;
                        }
                        else
                        {
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwBR;
                            pStaticData->IndexList[iData][iIndex++] = dwBL;
                        }
                        if ((iData & 8) && (dwCol == 0))
                        {
                            DWORD dwLeftCenter = (dwTL + dwBL) / 2;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwBL;
                            pStaticData->IndexList[iData][iIndex++] = dwLeftCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwLeftCenter;
                            pStaticData->IndexList[iData][iIndex++] = dwTL;
                        }
                        else
                        {
                            pStaticData->IndexList[iData][iIndex++] = dwCP;
                            pStaticData->IndexList[iData][iIndex++] = dwBL;
                            pStaticData->IndexList[iData][iIndex++] = dwTL;
                        }
                    }
                    else
                    {
                        pStaticData->IndexList[iData][iIndex++] = dwCP;
                        pStaticData->IndexList[iData][iIndex++] = dwTL;
                        pStaticData->IndexList[iData][iIndex++] = dwTR;

                        pStaticData->IndexList[iData][iIndex++] = dwCP;
                        pStaticData->IndexList[iData][iIndex++] = dwTR;
                        pStaticData->IndexList[iData][iIndex++] = dwBR;

                        pStaticData->IndexList[iData][iIndex++] = dwCP;
                        pStaticData->IndexList[iData][iIndex++] = dwBR;
                        pStaticData->IndexList[iData][iIndex++] = dwBL;

                        pStaticData->IndexList[iData][iIndex++] = dwCP;
                        pStaticData->IndexList[iData][iIndex++] = dwBL;
                        pStaticData->IndexList[iData][iIndex++] = dwTL;
                    }


                }

            }



        }

        CreateIndexBuffer(pStaticData);
        m_vLodIndexList.push_back(pStaticData);
    }



}