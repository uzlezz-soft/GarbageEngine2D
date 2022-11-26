#pragma once

#include "Core/Base.h"
#include "Math/Vector4.h"

class Vector2;
class Vector3;
class Rect;

class GARBAGE_API GARBAGE_ALIGN(64) Matrix4 final
{
public:

    /*
    1, 0, 0, 0
    0, 1, 0, 0
    0, 0, 1, 0
    0, 0, 0, 1
    */
    static const Matrix4 Identity;

    union
    {
        float V[4][4];
        Vector4 M[4];
    };

    Matrix4(float v = 1.0f) : Matrix4(v, 0, 0, 0, 0, v, 0, 0, 0, 0, v, 0, 0, 0, 0, v) {}

    Matrix4(float m_00, float m_01, float m_02 = 0, float m_03 = 0,
        float m_10 = 0, float m_11 = 0, float m_12 = 0, float m_13 = 0,
        float m_20 = 0, float m_21 = 0, float m_22 = 0, float m_23 = 0,
        float m_30 = 0, float m_31 = 0, float m_32 = 0, float m_33 = 0);

    Matrix4(float n[4][4]);

    Matrix4(const Vector3& a1, const Vector3& a2, const Vector3& a3);

    Matrix4(const Vector4& a1, const Vector4& a2, const Vector4& a3);

    Matrix4(const Vector4& a1, const Vector4& a2, const Vector4& a3, const Vector4& a4);

    Matrix4(const Matrix4& matrix);

    Matrix4 operator+(const Matrix4& n) const;

    Matrix4& operator+=(const Matrix4& n);

    Matrix4 operator-(const Matrix4& n) const;

    Matrix4& operator-=(const Matrix4& n);

    Matrix4& operator=(const Matrix4& matrix);

    bool operator==(const Matrix4& n) const;

    Matrix4 operator*(const Matrix4& n) const;

    Vector3 operator*(const Vector3& v) const;

    Vector4 operator*(const Vector4& v) const;

    Matrix4 operator*(float f) const;

    Matrix4& operator*=(float f);

    Matrix4 operator/(float f) const;

    Matrix4& operator/=(float f);

    Matrix4 operator-() const;

    Matrix4 Transpose() const;

    Matrix4 Inverse() const;

    void SetAllValues(float a1, float a2, float a3, float a4,
        float b1, float b2, float b3, float b4,
        float c1, float c2, float c3, float c4,
        float d1, float d2, float d3, float d4);

    Matrix4& SetToIdentity();

    float* DataBlock() { return V[0]; }

    const float* DataBlock() const { return V[0]; }

    float Value(int i, int j) const { return V[i][j]; }

    float Trace() const { return (V[0][0] + V[1][1] + V[2][2] + V[3][3]); }

    Matrix4 Translate(const Vector3& v);

    Matrix4 Scale(const Vector3& v);

    Matrix4 RotateX(float angle);

    Matrix4 RotateY(float angle);

    Matrix4 RotateZ(float angle);

    Matrix4 Rotate(const Vector3& axis, float angle);

    static Matrix4 Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);

    static Matrix4 Perspective(const float fovy, const float aspect, const float zNear, const float zFar);

    static Matrix4 Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);

    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

    static Vector3 UnProject(const Vector3& vec, const Matrix4& view, const Matrix4& proj, const Rect& viewport);

    static Vector3 Project(const Vector3& vec, const Matrix4& view, const Matrix4& proj, const Rect& viewport);

};

GARBAGE_API inline Matrix4 operator*(float f, const Matrix4& m)
{
    return (m * f);
}
