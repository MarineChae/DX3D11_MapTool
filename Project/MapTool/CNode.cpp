#include "CNode.h"
#include"CoreInterface.h"
void CNode::SetParent(CNode* pParent)
{
    if (pParent == nullptr)return;
    m_pParent = pParent;
    m_iDepth = pParent->m_iDepth + 1;
}

bool CNode::CreateIndexBuffer()
{


    D3D11_BUFFER_DESC Desc;
    Desc.ByteWidth = sizeof(DWORD) * m_vIndexList.size();//������ ũ��
    Desc.Usage = D3D11_USAGE_DEFAULT;//���۸� ��𿡼� ����������
    Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//������ ���뵵
    Desc.CPUAccessFlags = 0;//cpu�� ���������� �ʿ������ null
    Desc.MiscFlags = 0;//�߰����� �ɼ�
    Desc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA pInitialData;
    pInitialData.pSysMem = &m_vIndexList.at(0);

    CoreInterface::g_pDevice->CreateBuffer(&Desc, &pInitialData, &m_pIndexBuffer);

    return true;

}

bool CNode::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC Desc;
    Desc.ByteWidth = sizeof(PNCT_VERTEX) * m_vVertexList.size();
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;
    Desc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA InitialData;
    InitialData.pSysMem = &m_vVertexList.at(0);


    CoreInterface::g_pDevice->CreateBuffer(
        &Desc,
        &InitialData,
        m_pVertexBuffer.GetAddressOf());


    return true;
}

bool CNode::Render()
{


    return true;
}

bool CNode::Release()
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
    return true;
}

CNode::CNode(CNode* pParent, DWORD LT, DWORD RT, DWORD LB, DWORD RB)
{

    m_pParent = pParent;
    m_CornerIndex.resize(4);
    m_CornerIndex[0] = LT;
    m_CornerIndex[1] = RT;
    m_CornerIndex[2] = LB;
    m_CornerIndex[3] = RB;




}
