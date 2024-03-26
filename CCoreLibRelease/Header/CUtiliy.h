#pragma once
#include"Tmath.h"
struct PNCT_VERTEX
{

    TVector3 Pos;
    TVector3 Nor;
    TVector4 Col;
    TVector2 Tex;

    PNCT_VERTEX() {}
    PNCT_VERTEX(TVector3		vp,
        TVector3		vn,
        TVector4		vc,
        TVector2     vt)
    {
        Pos = vp, Nor = vn, Col = vc, Tex = vt;
    }
    bool operator==(const PNCT_VERTEX& other) const
    {
        return Pos == other.Pos && Nor == other.Nor && Col == other.Col && Tex == other.Tex;
    }
};
struct LightDesc
{
    TVector4 Ambient;
    TVector4 Diffuse;
    TVector4 Specular;


};
struct ChangeEveryFrame
{
    TVector3 CameraPos;
    float    padding0;
    TVector3 CamereaDir;//look
    float    padding1;
    TVector3 LightDirection;
    float    padding2;
};
struct SHADOW_CONSTANT_BUFFER
{
    TMatrix			g_matShadow[2];
};

struct CUVRect
{
    TVector2 m_Min;
    TVector2 m_Max;

};

class CRect
{
public:
    bool m_bvalid = true;
    TVector2 m_pPosition;
    float m_Width = 0.0f;
    float m_height = 0.0f;
    TVector2 m_Point[4];
    TVector2 m_pCenter;
    TVector2 Size;
    TVector2 m_Max;
    TVector2 m_Min;
    TVector2 m_Half;
    float m_Angle;
public:
    CRect operator+(CRect& other);
    CRect operator - (CRect& other);
    CRect operator *(float value);
    CRect operator /(float value);
    bool operator == (CRect& other);
    bool operator !=(CRect& other);
    
public:
    bool ToRect(CRect& rect);
    bool ToRectFootHold(CRect& rect);
    void Set(TVector2 p, float width, float height);
    void SetRect(TVector2 p, float width, float height);
public:
    CRect();
    CRect(TVector2 p, float width, float height);
};



class CBox
{
public:
    bool m_bvalid = true;
    TVector3 m_pPosition;
    float m_Width = 0.0f;
    float m_height = 0.0f;
    float m_Depth = 0.0f;
    TVector3 m_Point[8];
    TVector3 m_pCenter;
    TVector3 Size;
    TVector3 m_Max;
    TVector3 m_Min;
    TVector3 m_Half;
    TVector3 m_vAxis[3];
    float m_fExtent[3];
public:
    CBox operator+(CBox& other);
    CBox operator - (CBox& other);
    CBox operator *(float value);
    CBox operator /(float value);
    bool operator == (CBox& other);
    bool operator !=(CBox& other);

public:
    bool ToBox(CBox& Box);
    void Set(TVector3 p, float width, float height, float depth);
public:
    CBox();
    CBox(TVector3 p, float width, float height, float depth);
};

class CPlane
{
public:
    TVector3 p1, p2, p3;

    float fA, fB, fC, fD;

public:
    bool CreatePlane(TVector3 v0, TVector3 v1, TVector3 v2);
    bool CreatePlane(TVector3 vNormal, TVector3 v0);
    void	Normalize()
    {
        float fMag = sqrt(fA * fA + fB * fB + fC * fC);
        fA = fA / fMag;
        fB = fB / fMag;
        fC = fC / fMag;
        fD = fD / fMag;
    }

};

struct CSphere
{
    TVector3 vCenter;
    float    fRadius;
};

struct Circle
{
    TVector2 vCenter;
    float    fRadius;
};


class CUtiliy
{



};

