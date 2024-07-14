#include "CObject.h"
#include "CShaderMgr.h"
#include"CTextureMgr.h"
#include"CoreInterface.h"
void CObject::SetRect(TVector2 p, float width, float height)
{
    m_RT.Set(p,width,height);

}

bool CObject::SetPos(TVector3 Pos)
{
    m_vPos = Pos;

    return true;
}

void CObject::SetScale(TVector3 Pos)
{
    m_vScale = Pos;
}



bool CObject::Create(std::wstring FileName, std::wstring ShaderFileName)
{
    CreateConstantBuffer();
    CreateVertexData();
    CreateIndexData();
    CreateIndexBuffer();
    CreateVertexBuffer();
    m_pShader = CShaderMgr::GetInstance().Load(ShaderFileName);
    CreateInputLayout();
    m_pTexture = CTextureMgr::GetInstance().Load(FileName);
    UpdataMatrix();
    return true;
}

//bool CObject::Create(std::wstring FileName, std::wstring ShaderFileName)
//{
//    CreateConstantBuffer();
//    CreateIndexBuffer();
//    CreateVertexBuffer();
//    m_pShader = CShaderMgr::GetInstance().Load(ShaderFileName);
//    CreateInputLayout();
//    m_pTexture = CTextureMgr::GetInstance().Load(FileName);
//    return true;
//}

void CObject::SetMatrix(TMatrix* WolrdMatrix, TMatrix* ViewMatrix, TMatrix* ProjMatrix)
{
    if (WolrdMatrix != nullptr)
    {
        m_WolrdMatrix = *WolrdMatrix;
    }
    if (ViewMatrix != nullptr)
    {
        m_ViewMatrix = *ViewMatrix;
    }
    if (ProjMatrix != nullptr)
    {
        m_ProjMatrix = *ProjMatrix;
    }

    m_ConstantData.WolrdMatrix = m_WolrdMatrix.Transpose();

    m_ConstantData.ViewMatrix = m_ViewMatrix.Transpose();

    m_ConstantData.ProjMatrix = m_ProjMatrix.Transpose();

    
    CoreInterface::g_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &m_ConstantData, 0, 0);

}




bool CObject::Init()
{
    
    return true;
}


bool CObject::Frame()
{
   /* CVector2 rt = { m_vPos.x,m_vPos.y };
    SetRect(rt, m_vScale.x*2.0f, m_vScale.y*2.0f);*/

    

    return true;
}

void CObject::UpdataMatrix()
{
    TMatrix matrixSCale;
    TMatrix matrixRotate;
    TMatrix matrixtrans;
    D3DXMatrixScaling(&matrixSCale, m_vScale.x, m_vScale.y, m_vScale.z);
    D3DXMatrixRotationZ(&matrixRotate, m_vRotate.z);
    D3DXMatrixTranslation(&matrixSCale, m_vPos.x, m_vPos.y, m_vPos.z);

    m_WolrdMatrix = matrixSCale * matrixRotate * matrixtrans;
}

bool CObject::Render()
{
    CDxObject::Render();
    return true;
}


bool CObject::Release()
{
   //delete m_pShader;
   //delete m_pTexture;
    CDxObject::Release();
    return true;
}
