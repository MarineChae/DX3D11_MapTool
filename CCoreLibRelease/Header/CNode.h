#pragma once

#include"CDefine.h"

#define DEPTH 1
#define TREESIZE 4


class CNode
{
public:

    CBox m_Box;
    CSphere m_Sphere;
    bool m_bIsLeaf = false;
    std::vector<TVector3> m_pCornerList;
    std::vector<DWORD>	m_CornerIndex;
    std::vector<CNode*> m_pChildList;
    std::vector<DWORD> m_vIndexList;
    std::vector<DWORD> m_vVertexIndexList;
    std::vector <PNCT_VERTEX> m_vVertexList;
    std::vector<CNode*>  m_vNeghborList;

    ComPtr<ID3D11Buffer> m_pIndexBuffer = nullptr;
    ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;
    int m_iCellRow;
    int m_iCellCol;

    DWORD m_dwPositionIndex[2];
    DWORD m_dwLodLevel;
    DWORD m_dwLodType;
    int m_iDepth = 0;
    CNode* m_pParent = nullptr;
   
public:

    void SetParent(CNode* pParent);
    bool Release();
    bool CreateIndexBuffer();
    bool CreateVertexBuffer();
    bool Render();
    CNode();
    CNode(CNode* pParent, DWORD LT, DWORD RT, DWORD LB, DWORD RB);
    ~CNode()
    {
        if (m_pChildList.size() != 0)
        {
            delete m_pChildList[0];
            delete m_pChildList[1];
            delete m_pChildList[2];
            delete m_pChildList[3];

        }
        m_pCornerList.clear();
        m_CornerIndex.clear();
        m_pChildList.clear();
        m_vIndexList.clear();
        m_vVertexList.clear();
       
    }
  
  




};