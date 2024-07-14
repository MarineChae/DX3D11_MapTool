#include "CDebugCamera.h"
#include"CInput.h"
bool CDebugCamera::Init()
{
    CCamera::Init();
    return true;
}

bool CDebugCamera::Frame()
{
    if (CInput::GetInstance().m_dwKeyState[VK_SPACE] == KEY_HOLD)
    {
        m_fSpeed += g_fSecondPerFrame * m_fSpeed;

    }
    m_fSpeed -= g_fSecondPerFrame * m_fSpeed * 0.5f;
    if (m_fSpeed >= 100)
    {
        m_fSpeed = 100;
    }
    if (m_fSpeed < 50)
    {
        m_fSpeed = 50;

    }
    if (CInput::GetInstance().m_dwKeyState['W'] == KEY_HOLD)
    {
       TVector3 offset =  m_vLook* g_fSecondPerFrame* m_fSpeed;
       m_vCameraPos = m_vCameraPos + offset;

    }
    if (CInput::GetInstance().m_dwKeyState['S'] == KEY_HOLD)
    {
        TVector3 offset = m_vLook * g_fSecondPerFrame * m_fSpeed;
        m_vCameraPos = m_vCameraPos - offset;

    }
    if (CInput::GetInstance().m_dwKeyState['A'] == KEY_HOLD)
    {
        TVector3 offset = m_vSide * g_fSecondPerFrame * m_fSpeed;
        m_vCameraPos = m_vCameraPos - offset;

    }
    if (CInput::GetInstance().m_dwKeyState['D'] == KEY_HOLD)
    {
        TVector3 offset = m_vSide * g_fSecondPerFrame * m_fSpeed;
        m_vCameraPos = m_vCameraPos + offset;

    }
    if (CInput::GetInstance().m_dwKeyState['Q'] == KEY_HOLD)
    {
        TVector3 offset = m_vUp * g_fSecondPerFrame * m_fSpeed;
        m_vCameraPos = m_vCameraPos + offset;

    }
    if (CInput::GetInstance().m_dwKeyState['E'] == KEY_HOLD)
    {
        TVector3 offset = m_vUp * g_fSecondPerFrame * m_fSpeed;
        m_vCameraPos = m_vCameraPos - offset;

    }
    if (CInput::GetInstance().m_dwKeyState[VK_LBUTTON] == KEY_HOLD)
    {
       float offsetX = CInput::GetInstance().m_vOffSet.x;
       float offsetY =  CInput::GetInstance().m_vOffSet.y;

       m_fCameraYaw += XMConvertToRadians(offsetX*0.1f);
       m_fCameraPitch += XMConvertToRadians(offsetY*0.1f);

    }
    TQuaternion QRotation;
    TMatrix MatRotation;
    D3DXQuaternionRotationYawPitchRoll(&QRotation, m_fCameraYaw, m_fCameraPitch, 0);
    D3DXMatrixAffineTransformation(&MatRotation, 1.0f, NULL, &QRotation, &m_vCameraPos);
    D3DXMatrixInverse(&m_ViewMatrix,NULL,&MatRotation);
    UpdateVector();
    m_WorldMatrix._41 = m_vCameraPos.x;
    m_WorldMatrix._42 = m_vCameraPos.y;
    m_WorldMatrix._43 = m_vCameraPos.z;

    return true;
}

bool CDebugCamera::Render()
{
    CCamera::Render();
    return true;
}

bool CDebugCamera::Release()
{
    CCamera::Release();
    return true;
}
