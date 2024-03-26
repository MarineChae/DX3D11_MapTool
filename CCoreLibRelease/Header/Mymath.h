#pragma once
#include <math.h>
#include <windows.h>

#include<string>
#include<vector>
#include<memory>
#include<map>
#include<list>
#include<unordered_map>
using namespace std;
#define PI (3.141592f)
#define T_EPSILON ( 0.0001f)
#define randstep(fmin,fmax) ((float)fmin + ((float)fmax-(float)fmin) * rand()/RAND_MAX)
#define RadianToDegree(Radian) (Radian * (180.0f / PI) )
#define DegreeTORadian(Degree) (Degree * (PI / 180.0f) )
//트리의 크기를 설정
#define MAXWIDTH 1000
#define MAXHEIGHT 1000
#define MAXDEPTH 1000

namespace MyMath
{



class CVector2;
class CVector3;
class CVector4;
class CMatrix;


struct TFloat2
{
    union
    {
        struct
        {

            float x;
            float y;


        };

        float v[2];

    };


};
struct TFloat3
{
    union
    {
        struct
        {

            float x;
            float y;
            float z;

        };

        float v[3];

    };


};
struct TFloat4
{
    union
    {
        struct
        {

            float x;
            float y;
            float z;
            float w;
        };

        float v[4];

    };


};
struct TFloat4x4
{
    union
    {
        struct
        {

            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };

        float m[4][4];

    };


};
class CVector2 : public TFloat2
{

public:

    bool operator ==(CVector2& other);
    bool operator !=(CVector2& other);
    CVector2 operator +(CVector2& other);
    CVector2 operator -(CVector2& other);
    CVector2 operator *(float other);
    CVector2& operator *=(float other);
    CVector2 operator /(float other);
    float operator|(CVector2& other);
public:
    float length();
    CVector2& Normal();
public:
    CVector2();
    CVector2(float fx, float fy);
};
class CVector3 : public TFloat3
{

public:

    bool operator ==(CVector3& other);
    bool operator !=(CVector3& other);
    CVector3 operator +(CVector3& other);
    CVector3 operator +=(CVector3& other);

    CVector3 operator -(CVector3& other);
    CVector3 operator *(float other);
    CVector3 operator *(CVector3& other);
    CVector3& operator *=(float other);
    CVector3 operator /(float other);
    float operator | (CVector3& other);
    CVector3 operator ^ (CVector3& other);
    CVector3 operator * (CMatrix& other);
public:
    float length();
    void Normalize();
    CVector3 NormalVector();
    float Angle(CVector3& other);

public:
    CVector3();
    CVector3(float fx, float fy, float fz);
};
class CVector4 : public TFloat4
{
public:
  
    CVector4 operator *(float other);
    CVector4& operator *=(float other);
public:
    CVector4()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 0.0f;
    }
    CVector4(float fx, float fy, float fz, float fw)
    {
        x = fx;
        y = fy;
        z = fz;
        w = fw;
    }
    ~CVector4()
    {

    }

};
class CRay
{
public:

    CVector3 vPosition;
    CVector3 vDirection;

    float GetDistance(CVector3& other)
    {
        CVector3 v = other - vPosition;
        float k = ((v | vDirection) / (vDirection | vDirection));
        CVector3 p = vDirection * k;
        CVector3 w = v - p;
        return w.length();


    }

    CRay() {}
    CRay(CVector3 p, CVector3 d)
    {
        vPosition = p;
        vDirection = d;

    }


};
class CPlane
{
public:

    CVector3 vPos;
    CVector3 vNor;
    float a, b, c, d;

    float GetDistance(CVector3& other)
    {
        return a * other.x + b * other.y + c * other.z + d;

    }

    CPlane() {}

    CPlane(CVector3 Pos, CVector3 Dir)
    {
        vNor = Dir.NormalVector();
        vPos = Pos;
        a = vNor.x;
        b = vNor.y;
        c = vNor.z;
        d = -(vNor | vPos);

    }
    CPlane(CVector3 p0, CVector3 p1, CVector3 p2)
    {
        vPos = p0;
        CVector3 e1 = p1 - p0;
        CVector3 e2 = p2 - p0;
        vNor = (e1 ^ e2).NormalVector();
        a = vNor.x;
        b = vNor.y;
        c = vNor.z;
        d = -(vNor | vPos);



    }


};

class CMatrix : public TFloat4x4
{public:
    CMatrix()
    {
        Identity();
    }
    void Identity()
    {
        _11 = _12 = _13 = _14 = 0.0f;
        _21 = _22 = _23 = _24 = 0.0f;
        _31 = _32 = _33 = _34 = 0.0f;
        _41 = _42 = _43 = _44 = 0.0f;
        _11 = _22 = _33 = _44 = 1.0f;
    }

    CMatrix operator * (CMatrix const& other)
    {
        CMatrix ret;
        for (int iColumn = 0; iColumn < 4; iColumn++)
        {
            for (int iRow = 0; iRow < 4; iRow++)
            {
                ret.m[iRow][iColumn] =
                    m[iRow][0] * other.m[0][iColumn] +
                    m[iRow][1] * other.m[1][iColumn] +
                    m[iRow][2] * other.m[2][iColumn] +
                    m[iRow][3] * other.m[3][iColumn];
            }
        }
        return ret;

    }
    CVector3 operator * (CVector3 const& v)
    {
        CVector4 ret;
        ret.w = 1.0f;
        ret.x = v.x * _11 + v.y * _21 + v.z * _31 + 1.0f * _41;
        ret.y = v.x * _12 + v.y * _22 + v.z * _32 + 1.0f * _42;
        ret.z = v.x * _13 + v.y * _23 + v.z * _33 + 1.0f * _43;
        ret.w = v.x * _14 + v.y * _24 + v.z * _34 + 1.0f * _44;

        return CVector3(ret.x, ret.y, ret.z);
    }

    float operator () (int iRow, int iCol) const 
    {
        return m[iRow][iCol];
    }
    float& operator () (int iRow, int iCol)
    {
        return m[iRow][iCol];
    }

    void Translation(const CVector3& other)
    {

        _41 = other.x;
        _42 = other.y;
        _43 = other.z;

    }
    void Translation(float x, float y, float z)
    {

        _41 = x;
        _42 = y;
        _43 = z;

    }
    void Scale(const CVector3& other)
    {
        _11=other.x;
        _22=other.y;
        _33=other.z;
    }
    void Scale(float x , float y , float z)
    {
        _11 = x;
        _22 = y;
        _33 = z;
    }

    void XRotate(float fRadian)
    {
        float fCos = cos(fRadian);
        float fSin = sin(fRadian);
        _22 = fCos; _23 = fSin;
        _32 = -fSin; _33 = fCos;


    }

    void YRotate(float fRadian)
    {
        float fCos = cos(fRadian);
        float fSin = sin(fRadian);
        _11 = fCos; _13 = -fSin;
        _31 = fSin; _33 = fCos;


    }

    void ZRotate(float fRadian)
    {
        float fCos = cos(fRadian);
        float fSin = sin(fRadian);
        _11 = fCos; _12 = fSin;
        _21 = -fSin; _22 = fCos;


    }

    CMatrix TransPose()
    {
        CMatrix ret;
        ret._11 = _11;  ret._12 = _21;  ret._13 = _31;  ret._14 = _41;
        ret._21 = _12;  ret._22 = _22;  ret._23 = _32;  ret._24 = _42;
        ret._31 = _13;  ret._32 = _23;  ret._33 = _33;  ret._34 = _43;
        ret._41 = _14;  ret._42 = _24;  ret._43 = _34;  ret._44 = _44;

        return ret;

    }


};

}

