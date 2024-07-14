#include "CUtiliy.h"
CRect CRect:: operator+(CRect& other)
{
    float fMinX = min(m_Min.x, other.m_Min.x);
    float fMinY = min(m_Min.y, other.m_Min.y);
    float fMaxX = max(m_Max.x, other.m_Max.x);
    float fMaxY = max(m_Max.y, other.m_Max.y);

    TVector2 Center = { (fMaxX + fMinX) * 0.5f, (fMinY + fMaxY) * 0.5f };
    CRect rt;
    rt.Set(Center, fMaxX - fMinX, fMaxY - fMinY);

    return rt;

}
CRect CRect::operator - (CRect& other)
{
    CRect rt;
    m_bvalid = false;

    if (ToRect(other))
    {
        float left = min(m_Min.x, other.m_Min.x);
        float top = min(m_Min.y, other.m_Min.y);
        float right = max(m_Max.x, other.m_Max.x);
        float bottom = max(m_Max.y, other.m_Max.y);

        rt.Set(TVector2(left, top), right - left, bottom - top);
        m_bvalid = true;

    }

    return rt;


}
CRect CRect::operator *(float value)
{
    m_pPosition.x *= value;
    m_pPosition.y *= value;

    return *this;
}
CRect CRect::operator /(float value)
{
    m_pPosition.x /= value;
    m_pPosition.y /= value;

    return *this;
}
bool CRect::operator == (CRect& other)
{
    if (m_pPosition.x == other.m_pPosition.x)
    {
        if (m_pPosition.y == other.m_pPosition.y)
        {
            if (m_Width == other.m_Width)
            {
                if (m_height == other.m_height)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
bool CRect::operator !=(CRect& other)
{
    return !(*this == other);
}
bool CRect::ToRect(CRect& rect)
{
    CRect rt = (*this) + rect;

    if (m_Width + rect.m_Width >= rt.m_Width)
    {
        if (m_height + rect.m_height >= rt.m_height)
        {
          
            return true;
        }
    }

    return false;

}
bool CRect::ToRectFootHold(CRect& rect)
{
    CRect rt = (*this) + rect;

    if (m_Width + rect.m_Width >= rt.m_Width)
    {
        if (m_height + rect.m_height >= rt.m_height)
        {
            if(m_Min.y >= rect.m_Min.y)
            return true;
        }
    }

    return false;


}

void CRect::Set(TVector2 p, float width, float height)
{
    m_pCenter = p;

    m_pPosition.x = p.x-(width*0.5f);
    m_pPosition.y = p.y+(height*0.5f);

    m_Width = width;
    m_height = height;
    m_Half = { m_Width * 0.5f,m_height * 0.5f };
    m_Point[0] = { m_pPosition.x,m_pPosition.y };
    m_Point[1] = { m_pPosition.x + m_Width,m_pPosition.y };
    m_Point[2] = { m_pPosition.x + m_Width,m_pPosition.y - m_height };
    m_Point[3] = { m_pPosition.x,m_pPosition.y - m_height };


    m_Min = m_Point[3];
    m_Max = m_Point[1];
  
  

}
void CRect::SetRect(TVector2 p, float width, float height)
{
    Set(p, width, height);
}
CRect::CRect()
{

}
CRect::CRect(TVector2 p, float width, float height)
{
    m_bvalid = true;


}

/// <summary>
/// 
/// </summary>
/// <param name="other"></param>
/// <returns></returns>
CBox CBox::operator+(CBox& other)
{
    float fMinX = min(m_pPosition.x, other.m_pPosition.x);
    float fMinY = min(m_pPosition.y, other.m_pPosition.y);
    float fMinZ = min(m_pPosition.z, other.m_pPosition.z);
    float fMaxX = max(m_pPosition.x + m_Width, other.m_pPosition.x + other.m_Width);
    float fMaxY = max(m_pPosition.y + m_height, other.m_pPosition.y + other.m_height);
    float fMaxZ = max(m_pPosition.z + m_Depth, other.m_pPosition.z + other.m_Depth);

    CBox box;
    box.Set(TVector3(fMinX, fMinY, fMinZ), fMaxX - fMinX, fMaxY - fMinY, fMaxZ - fMinZ);

    return box;

}
CBox CBox:: operator - (CBox& other)
{
    CBox rt;
    m_bvalid = false;

    if (ToBox(other))
    {
        float fx = min(m_Min.x, other.m_Min.x);
        float fy = min(m_Min.y, other.m_Min.y);
        float fz = min(m_Min.z, other.m_Min.z);
        float fSizeX = max(m_Max.x, other.m_Max.x);
        float fSizeY = max(m_Max.y, other.m_Max.y);
        float fSizeZ = max(m_Max.z, other.m_Max.z);


        rt.Set(TVector3(fx, fy, fz), fSizeX, fSizeY, fSizeZ);
        m_bvalid = true;

    }

    return rt;


}
CBox CBox::operator *(float value)
{
    m_pPosition.x *= value;
    m_pPosition.y *= value;

    return *this;
}
CBox CBox::operator /(float value)
{
    m_pPosition.x /= value;
    m_pPosition.y /= value;

    return *this;
}
bool CBox::operator == (CBox& other)
{
    if (m_pPosition.x == other.m_pPosition.x)
    {
        if (m_pPosition.y == other.m_pPosition.y)
        {
            if (m_Width == other.m_Width)
            {
                if (m_height == other.m_height)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
bool CBox::operator !=(CBox& other)
{
    return !(*this == other);
}
bool CBox::ToBox(CBox& Box)
{
    CBox box = *this + Box;

    if (m_Width + Box.m_Width >= box.m_Width)
    {
        if (m_height + Box.m_height >= box.m_height)
        {
            if (m_Depth + Box.m_Depth >= box.m_Depth)
            {
                return true;
            }
        }
    }



    return false;


}
//박스의 영역을 직교좌표계로 설정하는 함수
void CBox::Set(TVector3 p, float width, float height, float depth)
{

    m_pPosition = p;
    Size = { width,height,depth };
    m_Width = width;
    m_height = height;
    m_Depth = depth;
    m_Half = { m_Width * 0.5f,m_height * 0.5f,m_Depth * 0.5f };

    m_Point[0] = { p.x- m_Half.x ,p.y + m_Half.y ,p.z- m_Half.z };

    m_Point[1] = m_Point[0];
    m_Point[1].x += m_Width;

    m_Point[2] = m_Point[1];
    m_Point[2].y -= m_height;

    m_Point[3] = m_Point[2];
    m_Point[3].x = m_Point[0].x;

    m_Point[4] = { p.x - m_Half.x ,p.y + m_Half.y ,p.z + m_Half.z };

    m_Point[5] = m_Point[4];
    m_Point[5].x += m_Width;

    m_Point[6] = m_Point[5];
    m_Point[6].y -= m_height;

    m_Point[7] = m_Point[6];
    m_Point[7].x = m_Point[4].x;

   


    m_Min = { m_Point[3] };
    m_Max = { m_Point[5] };
    m_pCenter = { (m_Min + m_Max) *0.5f};
    Size = { width ,height,depth };

}
CBox::CBox()
{

}
CBox::CBox(TVector3 p, float width, float height, float depth)
{
    m_bvalid = true;
    Set(p, width, height, depth);


}