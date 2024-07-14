#include "Mymath.h"

namespace MyMath
{
    bool CVector2::operator ==(CVector2& other)
    {
        if (fabs(x - other.x) < 0.0001f)
        {
            if (fabs(y - other.y) < 0.0001f)
            {
                return true;
            }
        }

        return false;
    }
    bool CVector2::operator !=(CVector2& other)
    {
        return !(*this == other);
    }
    CVector2 CVector2::operator +(CVector2& other)
    {
        return CVector2(x + other.x, y + other.y);
    }
    CVector2 CVector2::operator -(CVector2& other)
    {
        return CVector2(x - other.x, y - other.y);
    }
    CVector2 CVector2::operator *(float other)
    {

        return CVector2(x * other, y * other);
    }
    float CVector2::operator|(CVector2& other)
    {
        return x * other.x + y * other.y;
    }
    CVector2& CVector2::operator *=(float other)
    {

        x *= other;
        y *= other;
        return *this;
    }
    CVector2 CVector2::operator /(float other)
    {

        return CVector2(x / other, y / other);

    }
    float CVector2::length()
    {
        double size;
        size = sqrt(pow(x, 2) + pow(y, 2));
        //float fDistance = sqrt(x * x + y * y);
        return size;
    }

    CVector2& CVector2::Normal()
    {
        float Length = 1.0f / length();
        x *= Length;
        y *= Length;
        return *this;
    }

    CVector2::CVector2()
    {

    }
    CVector2::CVector2(float fx, float fy)
    {
        x = fx;
        y = fy;

    }

    bool CVector3::operator ==(CVector3& other)
    {
        if (fabs(x - other.x) < 0.0001f)
        {
            if (fabs(y - other.y) < 0.0001f)
            {
                if (fabs(z - other.z) < 0.0001f)
                {
                    return true;
                }
            }
        }

        return false;
    }
    bool CVector3::operator !=(CVector3& other)
    {
        return !(*this == other);
    }
    CVector3 CVector3::operator +(CVector3& other)
    {
        return CVector3(x + other.x, y + other.y, z + other.z);
    }
    CVector3 CVector3::operator +=(CVector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;

    }

    CVector3 CVector3::operator -(CVector3& other)
    {
        return CVector3(x - other.x, y - other.y, z - other.z);
    }
    CVector3 CVector3::operator *(float other)
    {

        return CVector3(x * other, y * other, z * other);
    }
    CVector3 CVector3::operator*(CVector3& other)
    {


        return CVector3(x * other.x, y * other.y, z * other.z);
    }
    CVector3& CVector3::operator *=(float other)
    {

        x *= other;
        y *= other;
        z *= other;
        return *this;
    }
    CVector3 CVector3::operator /(float other)
    {

        return CVector3(x / other, y / other, z / other);

    }

    float CVector3::operator|(CVector3& other)
    {
        return x * other.x + y * other.y + z * other.z;
    }
    CVector3 CVector3::operator ^ (CVector3& other)
    {

        return CVector3(y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.z);

    }

    CVector3 CVector3::operator*(CMatrix& other)
    {
        CVector4 t;
        t.x = x * other._11 + y * other._21 + z * other._31 + 1.0f * other._41;
        t.y = x * other._12 + y * other._22 + z * other._32 + 1.0f * other._42;
        t.z = x * other._13 + y * other._23 + z * other._33 + 1.0f * other._43;
        t.w = x * other._14 + y * other._24 + z * other._34 + 1.0f * other._44;

        if (fabs(t.w - 0.0f) > T_EPSILON)
        {
            t.x /= t.w;
            t.y /= t.w;
            t.z /= t.w;
            t.w /= t.w;
        }
        return CVector3(t.x, t.y, t.z);
    }

    //길이를 구하는함수
    float CVector3::length()
    {
        float fDistance = sqrt(x * x + y * y + z * z);
        return fDistance;
    }

    //벡터 정규화함수
    void CVector3::Normalize()
    {
        float unitlength = 1.0f / length();
        x = x * unitlength;
        y = y * unitlength;
        z = z * unitlength;

    }

    //벡터 정규화함수
    CVector3 CVector3::NormalVector()
    {
        float UnitLength = 1.0f / length();

        return (*this) * UnitLength;
    }

    //a와b 벡터 사이의 각을 구하는 함수 
    float CVector3::Angle(CVector3& other)
    {
        CVector3 a = NormalVector();
        CVector3 b = other.NormalVector();
        float Cos = a | b;
        float Radian = acos(Cos);
        float Degree = RadianToDegree(Radian);

        return Degree;
    }

    CVector3::CVector3()
    {

    }
    CVector3::CVector3(float fx, float fy, float fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }

    CVector4 CVector4::operator*(float other)
    {
        return CVector4(x * other, y * other, z * other, w * other);
        // TODO: 여기에 return 문을 삽입합니다.
    }

    CVector4& CVector4::operator*=(float other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
        return (*this);
        // TODO: 여기에 return 문을 삽입합니다.
    }
}
