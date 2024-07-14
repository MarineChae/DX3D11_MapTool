#include "CFrustum.h"
#include"CCollision.h"


bool CPlane::CreatePlane(TVector3 v0, TVector3 v1, TVector3 v2)
{
    p1 = v0;
    p2 = v1;
    p3 = v2;

    TVector3 Edge0 = v1 - v0;
    TVector3 Edge1 = v2 - v0;
    TVector3 vNormal;
    D3DXVec3Cross(&vNormal, &Edge0, &Edge1);
    D3DXVec3Normalize(&vNormal, &vNormal);

    fA = vNormal.x; fB = vNormal.y; fC = vNormal.z;
    fD = -(fA * v0.x + fB * v0.y + fC * v0.z);


    return true;
}

bool CPlane::CreatePlane(TVector3 vNormal, TVector3 v0)
{

    D3DXVec3Normalize(&vNormal, &vNormal);
    fA = vNormal.x; fB = vNormal.y; fC = vNormal.z;
    fD = -(fA * v0.x + fB * v0.y + fC * v0.z);

    return true;
}



bool CFrustum::CreateRenderBox()
{


  

    return false;
}
void CFrustum::SetMatrix(TMatrix* pWorld, TMatrix* pView, TMatrix* pProj)
{
   
}
void CFrustum::ExtractPlanesD3D(CPlane* pPlanes, const TMatrix& comboMatrix)
{
    if (pPlanes == NULL) return;
    // Left clipping plane
    pPlanes[0].fA = comboMatrix._14 + comboMatrix._11;
    pPlanes[0].fB = comboMatrix._24 + comboMatrix._21;
    pPlanes[0].fC = comboMatrix._34 + comboMatrix._31;
    pPlanes[0].fD = comboMatrix._44 + comboMatrix._41;
    // Right clipping plane
    pPlanes[1].fA = comboMatrix._14 - comboMatrix._11;
    pPlanes[1].fB = comboMatrix._24 - comboMatrix._21;
    pPlanes[1].fC = comboMatrix._34 - comboMatrix._31;
    pPlanes[1].fD = comboMatrix._44 - comboMatrix._41;
    // Top clipping plane
    pPlanes[2].fA = comboMatrix._14 - comboMatrix._12;
    pPlanes[2].fB = comboMatrix._24 - comboMatrix._22;
    pPlanes[2].fC = comboMatrix._34 - comboMatrix._32;
    pPlanes[2].fD = comboMatrix._44 - comboMatrix._42;
    // Bottom clipping plane
    pPlanes[3].fA = comboMatrix._14 + comboMatrix._12;
    pPlanes[3].fB = comboMatrix._24 + comboMatrix._22;
    pPlanes[3].fC = comboMatrix._34 + comboMatrix._32;
    pPlanes[3].fD = comboMatrix._44 + comboMatrix._42;
    // Near clipping plane
    pPlanes[4].fA = comboMatrix._13;
    pPlanes[4].fB = comboMatrix._23;
    pPlanes[4].fC = comboMatrix._33;
    pPlanes[4].fD = comboMatrix._43;
    // Far clipping plane
    pPlanes[5].fA = comboMatrix._14 - comboMatrix._13;
    pPlanes[5].fB = comboMatrix._24 - comboMatrix._23;
    pPlanes[5].fC = comboMatrix._34 - comboMatrix._33;
    pPlanes[5].fD = comboMatrix._44 - comboMatrix._43;

    pPlanes[0].Normalize();
    pPlanes[1].Normalize();
    pPlanes[2].Normalize();
    pPlanes[3].Normalize();
    pPlanes[4].Normalize();
    pPlanes[5].Normalize();
}
bool CFrustum::CreateFrustum(TMatrix ViewMat, TMatrix ProjMat)
{

    //5  6
    //4  7
    // 
    //1  2
    //0  3
    m_vFrustumPoint[0] = TVector3(-1.0f, -1.0f, 0.0f);
    m_vFrustumPoint[1] = TVector3(-1.0f, 1.0f, 0.0f);
    m_vFrustumPoint[2] = TVector3(1.0f, 1.0f, 0.0f);
    m_vFrustumPoint[3] = TVector3(1.0f, -1.0f, 0.0f);

    m_vFrustumPoint[4] = TVector3(-1.0f, -1.0f, 1.0f);
    m_vFrustumPoint[5] = TVector3(-1.0f, 1.0f, 1.0f);
    m_vFrustumPoint[6] = TVector3(1.0f, 1.0f, 1.0f);
    m_vFrustumPoint[7] = TVector3(1.0f, -1.0f, 1.0f);

    TMatrix Mat = ViewMat * ProjMat;
    D3DXMatrixInverse(&Mat, NULL, &Mat);

    for (int i = 0; i < 8; ++i)
    {
        D3DXVec3TransformCoord(&m_vFrustumPoint[i], &m_vFrustumPoint[i],&Mat);

    }
  
   //5  6
   //4  7
   // 
   //1  2
   //0  3

    //좌 우  상 하 앞 뒤
   m_Plane[0].CreatePlane(m_vFrustumPoint[0], m_vFrustumPoint[1], m_vFrustumPoint[5]);
   m_Plane[1].CreatePlane(m_vFrustumPoint[2], m_vFrustumPoint[3], m_vFrustumPoint[6]);
   m_Plane[2].CreatePlane(m_vFrustumPoint[5], m_vFrustumPoint[2], m_vFrustumPoint[6]);
   m_Plane[3].CreatePlane(m_vFrustumPoint[0], m_vFrustumPoint[7], m_vFrustumPoint[3]);
   m_Plane[4].CreatePlane(m_vFrustumPoint[0], m_vFrustumPoint[2], m_vFrustumPoint[1]);
   m_Plane[5].CreatePlane(m_vFrustumPoint[6], m_vFrustumPoint[4], m_vFrustumPoint[5]);

    
                                                                 

    return true;
}

bool CFrustum::ClassifyPoint(TVector3 v)
{
    float PlanetoPoint;
    for (int iPlane = 0; iPlane < 6; iPlane++)
    {
        PlanetoPoint = m_Plane[iPlane].fA * v.x + m_Plane[iPlane].fB * v.y + m_Plane[iPlane].fC * v.z + m_Plane[iPlane].fD;

        if (PlanetoPoint < 0.0f)
        {
            return false;
        }


    }


    return true;


}

bool CFrustum::CullingObject(CBox Box)
{
    for (int iPlane = 0; iPlane < 6; ++iPlane)
    {
        if (!CCollision::BoxToPlane(Box, m_Plane[iPlane]))
        {
            return false;
        }
    }
   
    return true;
}

//bool CFrustum::PreRender()
//{
//    m_vVertexList[0] = PNCT_VERTEX(m_vFrustum[1],TVector3(0.0f,0.0f,-1.0f),TVector4(1.0f,0.0f,0.0f,0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[1] = PNCT_VERTEX(m_vFrustum[2],TVector3(0.0f,0.0f,-1.0f),TVector4(1.0f,0.0f,0.0f,0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[2] = PNCT_VERTEX(m_vFrustum[3],TVector3(0.0f,0.0f,-1.0f),TVector4(1.0f,0.0f,0.0f,0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[3] = PNCT_VERTEX(m_vFrustum[1],TVector3(0.0f,0.0f,-1.0f),TVector4(1.0f,0.0f,0.0f,0.5f), TVector2(0.0f, 1.0f));
//
//    m_vVertexList[4] = PNCT_VERTEX(m_vFrustum[6], TVector3(0.0f, 0.0f, 1.0f), TVector4(0.0f, 1.0f, 0.0f, 0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[5] = PNCT_VERTEX(m_vFrustum[5], TVector3(0.0f, 0.0f, 1.0f), TVector4(0.0f, 1.0f, 0.0f, 0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[6] = PNCT_VERTEX(m_vFrustum[4], TVector3(0.0f, 0.0f, 1.0f), TVector4(0.0f, 1.0f, 0.0f, 0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[7] = PNCT_VERTEX(m_vFrustum[7], TVector3(0.0f, 0.0f, 1.0f), TVector4(0.0f, 1.0f, 0.0f, 0.5f), TVector2(0.0f, 1.0f));
//
//    m_vVertexList[8] = PNCT_VERTEX(m_vFrustum[5], TVector3(1.0f, 0.0f, 0.0f), TVector4(0.0f, 0.0f, 1.0f, 0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[9] = PNCT_VERTEX(m_vFrustum[1], TVector3(1.0f, 0.0f, 0.0f), TVector4(0.0f, 0.0f, 1.0f, 0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[10] = PNCT_VERTEX(m_vFrustum[0], TVector3(1.0f, 0.0f, 0.0f), TVector4(0.0f, 0.0f, 1.0f, 0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[11] = PNCT_VERTEX(m_vFrustum[4], TVector3(1.0f, 0.0f, 0.0f), TVector4(0.0f, 0.0f, 1.0f, 0.5f), TVector2(0.0f, 1.0f));
//
//    m_vVertexList[12] = PNCT_VERTEX(m_vFrustum[2], TVector3(-1.0f, 0.0f, 0.0f), TVector4(1.0f, 0.0f, 1.0f, 0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[13] = PNCT_VERTEX(m_vFrustum[6], TVector3(-1.0f, 0.0f, 0.0f), TVector4(1.0f, 0.0f, 1.0f, 0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[14] = PNCT_VERTEX(m_vFrustum[7], TVector3(-1.0f, 0.0f, 0.0f), TVector4(1.0f, 0.0f, 1.0f, 0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[15] = PNCT_VERTEX(m_vFrustum[3], TVector3(-1.0f, 0.0f, 0.0f), TVector4(1.0f, 0.0f, 1.0f, 0.5f), TVector2(0.0f, 1.0f));
//
//    m_vVertexList[16] = PNCT_VERTEX(m_vFrustum[5], TVector3(0.0f, 1.0f, 0.0f), TVector4(1.0f, 1.0f, 0.0f, 0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[17] = PNCT_VERTEX(m_vFrustum[6], TVector3(0.0f, 1.0f, 0.0f), TVector4(1.0f, 1.0f, 0.0f, 0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[18] = PNCT_VERTEX(m_vFrustum[2], TVector3(0.0f, 1.0f, 0.0f), TVector4(1.0f, 1.0f, 0.0f, 0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[19] = PNCT_VERTEX(m_vFrustum[1], TVector3(0.0f, 1.0f, 0.0f), TVector4(1.0f, 1.0f, 0.0f, 0.5f), TVector2(0.0f, 1.0f));
//
//    m_vVertexList[20] = PNCT_VERTEX(m_vFrustum[0], TVector3(0.0f, -1.0f, 0.0f), TVector4(0.0f, 1.0f, 1.0f, 0.5f), TVector2(0.0f, 0.0f));
//    m_vVertexList[21] = PNCT_VERTEX(m_vFrustum[3], TVector3(0.0f, -1.0f, 0.0f), TVector4(0.0f, 1.0f, 1.0f, 0.5f), TVector2(1.0f, 0.0f));
//    m_vVertexList[22] = PNCT_VERTEX(m_vFrustum[7], TVector3(0.0f, -1.0f, 0.0f), TVector4(0.0f, 1.0f, 1.0f, 0.5f), TVector2(1.0f, 1.0f));
//    m_vVertexList[23] = PNCT_VERTEX(m_vFrustum[4], TVector3(0.0f, -1.0f, 0.0f), TVector4(0.0f, 1.0f, 1.0f, 0.5f), TVector2(0.0f, 1.0f));
//
//
//   // CoreInterface::g_pImmediateContext->UpdateSubresource(m_pBoxShape->)
//
//
//
//    return true;
//
//}
//
//bool CFrustum::PostRedner()
//{
//    return false;
//}


BOOL CFrustum::ClassifyOBB(CBox* Box)
{
    TVector3 vDir;
    float fDist = 0.0f;
    float fPlaneToCenter = 0.0f;
    for (int i = 0; i < 6; i++)
    {
    //
    //    TVector3 cubePosVec = Box->m_pCenter - m_Plane[i].p1;
    //    
    //    TVector3 planeVec = m_Plane[i].p1 - m_Plane[i].p2;
    //    planeVec.Normalize();
    //    TVector3 distanceVec = cubePosVec - (planeVec * (cubePosVec.Dot(planeVec)));
    //    // 3 : left bottom front
    //    // 2 : right bottom front
    //    // 0 : left top front
    //    // 7 : left bottom back
    //    TVector3 c3, c2, c0, c7;
    //    c3 = Box->m_Min; 
    //    c2 = Box->m_Min; c2.x += Box->m_Width;
    //    c0 = c3; c0.y += Box->m_height;
    //    c7 = c3; c7.z += Box->m_Depth;
    //    TVector3 cubeVecX = (c2 - c3) * 0.5f;
    //    TVector3 cubeVecY = (c0 - c3) * 0.5f;
    //    TVector3 cubeVecZ = (c7 - c3) * 0.5
    //    TVector3 planeNormal(m_Plane[i].fA, m_Plane[i].fB, m_Plane[i].fC);
    //    float sum = (cubeVecX.Dot(planeNormal)) + (cubeVecY.Dot(planeNormal)) + (cubeVecZ.Dot(planeNormal));
    //    float dotResult = distanceVec.Dot(planeNormal);
    //    if (fabs(sum) >= fabs(dotResult))
    //        continue;
    //    if (0 > dotResult)
    //        return 0;

       vDir = Box->m_vAxis[0] * Box->m_fExtent[0];
       fDist = fabs(m_Plane[i].fA * vDir.x + m_Plane[i].fB * vDir.y + m_Plane[i].fC * vDir.z);
       vDir = Box->m_vAxis[1] * Box->m_fExtent[1];
       fDist += fabs(m_Plane[i].fA * vDir.x + m_Plane[i].fB * vDir.y + m_Plane[i].fC * vDir.z);
       vDir = Box->m_vAxis[2] * Box->m_fExtent[2];
       fDist += fabs(m_Plane[i].fA * vDir.x + m_Plane[i].fB * vDir.y + m_Plane[i].fC * vDir.z);
       fPlaneToCenter = m_Plane[i].fA * Box->m_pCenter.x + m_Plane[i].fB * Box->m_pCenter.y +
                        m_Plane[i].fC * Box->m_pCenter.z + m_Plane[i].fD;
     //걸쳐있는 경우 간혹 렌더링이 안되는 상황이 발생
     //범위를 여유롭게 잡아 어느정도 해결
      if (fPlaneToCenter+4.0f <= -fDist)
      {  
          return false;
      }


    }
    return  true;
}

BOOL CFrustum::ClassifySphere(CSphere* sphere)
{
    float fDist = 0.0f;
    float fPlaneToCenter = 0.0f;
    for (int i = 0; i < 6; i++)
    {
        fPlaneToCenter = m_Plane[i].fA * sphere->vCenter.x + m_Plane[i].fB * sphere->vCenter.y + m_Plane[i].fC * sphere->vCenter.z + m_Plane[i].fD;

        if (fPlaneToCenter <= sphere->fRadius)
        {
            return false;
        }
    }
    return true;
}
