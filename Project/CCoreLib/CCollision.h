#pragma once
#include "CUtiliy.h"

// �浹Ÿ��üũ
enum class CollisionType
{
    _OUTSIDE = 0,
    _INSIDE = 1,
    _SPANNING = 2

};


class CCollision
{
public:
    //���� �����浹
    static bool PointToRect(CRect& rt, TVector2 p)
    {
        if (rt.m_Min.x <= p.x && rt.m_Max.x >= p.x
            &&
            rt.m_Min.y <= p.y && rt.m_Max.y >= p.y)
            return true;

        return false;



    }
    //������ ������ �浹
    static bool RectToRect(CRect& rt1, CRect& rt2)
    {
        CRect rt = rt1 + rt2;

        if (rt1.m_Width + rt2.m_Width >= rt.m_Width)
        {
            if (rt1.m_height + rt2.m_height >= rt.m_height)
            {
                return true;
            }
        }





        return false;



    }
    //�ڽ��� �ڽ����浹
    static CollisionType BoxToBox(CBox& Box1, CBox& Box2)
    {
        //�⺻�� �浹���� �������� ����
        CollisionType ret = CollisionType::_OUTSIDE;
        //�ڽ��� ���ԵǾ��ִ��� Ȯ��
        if (BoxInBox(Box1, Box2))
        {   
            ret = CollisionType::_INSIDE;
            return ret;

        }

        TVector3 Direction = Box1.m_pCenter - Box2.m_pCenter;
       
        //�ڽ����� �Ÿ��� ���� �� ���� ������ǥ�踦 ����߱� ������
        //������ ����ؼ� ������ ������ ���� ����� �̻�����
        if (fabs(Direction.x) <= (Box1.m_Half.x + Box2.m_Half.x))
        {
            if (fabs(Direction.y) <= (Box1.m_Half.y + Box2.m_Half.y))
            {
                if (fabs(Direction.z) <= (Box1.m_Half.z + Box2.m_Half.z))
                {
                    ret = CollisionType::_SPANNING;
                    return ret;
                }
            }
        }




        return ret;


    }
     static bool BoxToPlane(CBox& Box, CPlane& Plane)
    {
       
         float fDist = 0.0f;
         float fPlaneToCenter = 0.0f;

         TVector3 vDir;

        /* vDir = Box.m_vAxis[0] * Box.m_fExtent[0];
         fDist += fabs(Plane.fA * vDir.x) + fabs(Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);
         vDir = Box.m_vAxis[1] * Box.m_fExtent[1];
         fDist += fabs(Plane.fA * vDir.x )+fabs( Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);
         vDir = Box.m_vAxis[2] * Box.m_fExtent[2];
         fDist += fabs(Plane.fA * vDir.x )+fabs( Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);*/
         
         vDir = Box.m_vAxis[0] * Box.m_fExtent[0];
         fDist += fabs(Plane.fA * vDir.x) + fabs(Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);
         vDir = Box.m_vAxis[1] * Box.m_fExtent[1];
         fDist += fabs(Plane.fA * vDir.x) + fabs(Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);
         vDir = Box.m_vAxis[2] * Box.m_fExtent[2];
         fDist += fabs(Plane.fA * vDir.x) + fabs(Plane.fB * vDir.y) + fabs(Plane.fC * vDir.z);
         
         fPlaneToCenter = Plane.fA * Box.m_pCenter.x + Plane.fB * Box.m_pCenter.y + Plane.fC * Box.m_pCenter.z+Plane.fD;
         if (fPlaneToCenter + 1.0f < -fDist)
         {
             return false;
         }
        //if(fPlaneToCenter<=-fDist)
        //{
        //    return false;
        //}
         
       

        return true;
    }
     static bool SphereToSphere(CSphere& s1, CSphere& s2)
     {
         TVector3 d = s1.vCenter - s2.vCenter;
         float fDistance = d.Length();
         if (fDistance <= (s1.fRadius + s2.fRadius))
         {
             return true;
         }
         return false;
     }
     static bool SphereToPlane(CSphere& sphere, CPlane& Plane)
     {
         float fDist = 0.0f;
         float fPlaneToCenter = 0.0f;

         fPlaneToCenter = Plane.fA * sphere.vCenter.x + Plane.fB * sphere.vCenter.y + Plane.fC * sphere.vCenter.z + Plane.fD;

         if (fPlaneToCenter <= sphere.fRadius)
         {
             return false;
         }

         return true;
     }




    //�ڽ��� ���ԵǾ��ִ��� Ȯ��
    //������Ʈ�� Ʈ���� ��忡 ���ԵǴ��� Ȯ���ϴ� �۾�
    static bool BoxInBox(CBox& Box1, CBox& Box2)
    {
        //1�� �ڽ��� �ּҰ����� 2���� �ּҰ��� ũ�� 1���� �ִ뺸�� 2���� �ִ밪�� ������
        //1�� �ڽ��� 2���ڽ��� ���ԵǾ��ִ�
        if (Box1.m_Min.x <= Box2.m_Min.x && Box1.m_Max.x >= Box2.m_Max.x)
        {
            if (Box1.m_Min.y <= Box2.m_Min.y && Box1.m_Max.y >= Box2.m_Max.y)
            {
                if (Box1.m_Min.z <= Box2.m_Min.z && Box1.m_Max.z >= Box2.m_Max.z)
                {
                    return true;
                }
            }

        }

        return false;

    }

    static bool PointInCircle(Circle cir, TVector2 pt)
    {

        TVector2 d = pt - cir.vCenter;
        float fDistance = d.Length();
        if (fDistance <= cir.fRadius)
        {
            return true;
        }


        return false;
    }
    static bool CircleToBox(Circle cir, CBox box)
    {
        CRect rt;
        rt.m_pCenter = TVector2(box.m_pCenter.x,box.m_pCenter.z);
        rt.m_Point[0]=TVector2(box.m_Min.x- cir.fRadius,box.m_Min.z - cir.fRadius);
        rt.m_Point[1]=TVector2(box.m_Min.x- cir.fRadius,box.m_Max.z + cir.fRadius);
        rt.m_Point[2]=TVector2(box.m_Max.x+ cir.fRadius,box.m_Max.z + cir.fRadius);
        rt.m_Point[3]=TVector2(box.m_Max.x+ cir.fRadius,box.m_Min.z - cir.fRadius);


        if (rt.m_Point[0].x < cir.vCenter.x && rt.m_Point[3].x > cir.vCenter.x
            && rt.m_Point[0].y < cir.vCenter.y && rt.m_Point[1].y > cir.vCenter.y)
        {
            return true;
        }
       /* else
        {
            if (PointInCircle(cir, rt.m_Point[0]))
                return true;
            if (PointInCircle(cir, rt.m_Point[1]))
                return true;
            if (PointInCircle(cir, rt.m_Point[2]))
                return true;
            if (PointInCircle(cir, rt.m_Point[3]))
                return true;
        }*/


        return false;

    }



};
