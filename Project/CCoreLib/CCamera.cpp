#include "CCamera.h"

bool CCamera::CreateCamera(TVector3 Pos, TVector2 Size)
{
    m_vCameraPos = Pos;
    m_dwWindowWidth = Size.x;
    m_dwWindowHeight = Size.y;

    return true;

}

TMatrix CCamera::ViewLookAtLH(TVector3 vPos, TVector3 VTarget, TVector3 vUp)
{
    m_vTargetPos = VTarget;
    m_vCameraPos = vPos;
    TVector3 vDir = VTarget - vPos;
    D3DXVec3Normalize(&vDir,&vDir);
    float fDot = D3DXVec3Dot(&vUp, &vDir);
    TVector3 w = vDir * fDot;
    TVector3 UpVector = vUp - w;
    D3DXVec3Normalize(&UpVector, &UpVector);
   // UpVector.Normalize();
    TVector3 vRightVector;
    D3DXVec3Cross(&vRightVector, &UpVector, &vDir);



    m_ViewMatrix._11 = vRightVector.x;  m_ViewMatrix._12 = UpVector.x;  m_ViewMatrix._13 = vDir.x;
    m_ViewMatrix._21 = vRightVector.y;  m_ViewMatrix._22 = UpVector.y;  m_ViewMatrix._23 = vDir.y;
    m_ViewMatrix._31 = vRightVector.z;  m_ViewMatrix._32 = UpVector.z;  m_ViewMatrix._33 = vDir.z;

    m_ViewMatrix._41 = -(vPos.x * m_ViewMatrix._11 + vPos.y * m_ViewMatrix._21 + vPos.z * m_ViewMatrix._31);
    m_ViewMatrix._42 = -(vPos.x * m_ViewMatrix._12 + vPos.y * m_ViewMatrix._22 + vPos.z * m_ViewMatrix._32);
    m_ViewMatrix._43 = -(vPos.x * m_ViewMatrix._13 + vPos.y * m_ViewMatrix._23 + vPos.z * m_ViewMatrix._33);

    UpdateVector();

    return m_ViewMatrix;
}

TMatrix CCamera::PerspectiveFovLH(float NearDist, float FarDist, float Fov, float Aspect)
{
    //float H, W, Q;

    //H = 1/tan(Fov * 0.5f);
    //W = H / Aspect;
    //Q = FarDist / (FarDist - NearDist);

    //m_ProjMatrix._44 = 0.0f;
    //m_ProjMatrix._11 = W;
    //m_ProjMatrix._22 = H;
    //m_ProjMatrix._33 = Q;
    //m_ProjMatrix._43 = -Q * NearDist;
    //m_ProjMatrix._34 = 1;
    m_fFar = FarDist;
    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, Fov, Aspect, NearDist, FarDist);

    return m_ProjMatrix;
}

void CCamera::UpdateVector()
{
    m_vLook.x = m_ViewMatrix._13;
    m_vLook.y = m_ViewMatrix._23;
    m_vLook.z = m_ViewMatrix._33;
    m_vUp.x = m_ViewMatrix._12;
    m_vUp.y = m_ViewMatrix._22;
    m_vUp.z = m_ViewMatrix._32;
    m_vSide.x = m_ViewMatrix._11;
    m_vSide.y = m_ViewMatrix._21;
    m_vSide.z = m_ViewMatrix._31;
   // D3DXVec3Normalize(&m_vLook, &m_vLook);
    CreateFrustum(m_ViewMatrix,m_ProjMatrix);
}
TMatrix CCamera::CreateProjMatrix(float fNear, float fFar, float fFov, float fAspect)
{
    m_fAspect = fAspect;
    m_fFov = fFov;
    m_fNear = fNear;
    m_fFar = fFar;
    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, m_fFov, m_fAspect, m_fNear, m_fFar);
    return m_ProjMatrix;
}
bool CCamera::Init()
{
    return true;
}

bool CCamera::Frame2D()
{

    if (m_vCameraPos.x <= -(Map_XSize)+(gWindow_Width))
    { 

        m_vCameraPos.x = -(Map_XSize ) + (gWindow_Width);

    }
    if (m_vCameraPos.y <= -(Map_YSize) + (gWindow_Height))
    {

        m_vCameraPos.y = -(Map_YSize ) + (gWindow_Height);

    }
    if (m_vCameraPos.x >= (Map_XSize ) - (gWindow_Width))
    {

        m_vCameraPos.x = (Map_XSize ) - (gWindow_Width);

    }
    if (m_vCameraPos.y >= (Map_YSize ) - (gWindow_Height))
    {

        m_vCameraPos.y = (Map_YSize ) - (gWindow_Height);

    }
    m_ViewMatrix._41 = -m_vCameraPos.x;
    m_ViewMatrix._42 = -m_vCameraPos.y;
    m_ViewMatrix._43 = -m_vCameraPos.z;
    m_OrthProjectionMatrix._11 = 2.0f / ((float)gWindow_Width);// *Zoom;
    m_OrthProjectionMatrix._22 = 2.0f / ((float)gWindow_Height);// *Zoom;
    return true;
}
bool CCamera::Frame()
{
  

    return true;

}
bool CCamera::Render()
{
    return true;
}
bool CCamera::Release()
{
  
   
    return true;
}
