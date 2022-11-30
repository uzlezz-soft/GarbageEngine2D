#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Rect.h"
#include "Core/Assert.h"
#include <limits>

GARBAGE_API const Matrix4 Matrix4::Identity = Matrix4
(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
);

Matrix4::Matrix4(float m_00, float m_01, float m_02, float m_03,
    float m_10, float m_11, float m_12, float m_13,
    float m_20, float m_21, float m_22, float m_23,
    float m_30, float m_31, float m_32, float m_33)
{
    V[0][0] = m_00; V[0][1] = m_01; V[0][2] = m_02;  V[0][3] = m_03;
    V[1][0] = m_10; V[1][1] = m_11; V[1][2] = m_12;  V[1][3] = m_13;
    V[2][0] = m_20; V[2][1] = m_21; V[2][2] = m_22;  V[2][3] = m_23;
    V[3][0] = m_30; V[3][1] = m_31; V[3][2] = m_32;  V[3][3] = m_33;
}

Matrix4::Matrix4(float n[4][4])
{
    V[0][0] = n[0][0]; V[0][1] = n[0][1]; V[0][2] = n[0][2]; V[0][3] = n[0][3];
    V[1][0] = n[1][0]; V[1][1] = n[1][1]; V[1][2] = n[1][2]; V[1][3] = n[1][3];
    V[2][0] = n[2][0]; V[2][1] = n[2][1]; V[2][2] = n[2][2]; V[2][3] = n[2][3];
    V[3][0] = n[3][0]; V[3][1] = n[3][1]; V[3][2] = n[3][2]; V[3][3] = n[3][3];
}

Matrix4::Matrix4(const Vector3& a1, const Vector3& a2, const Vector3& a3)
{
    V[0][0] = a1.X; V[0][1] = a2.X; V[0][2] = a3.X;  V[0][3] = 0.f;
    V[1][0] = a1.Y; V[1][1] = a2.Y; V[1][2] = a3.Y;  V[1][3] = 0.f;
    V[2][0] = a1.Z; V[2][1] = a2.Z; V[2][2] = a3.Z;  V[2][3] = 0.f;
    V[3][0] = 0.f;  V[3][1] = 0.f;  V[3][2] = 0.f;   V[3][3] = 1.f;
}

Matrix4::Matrix4(const Vector4& a1, const Vector4& a2, const Vector4& a3)
{
    V[0][0] = a1.X; V[0][1] = a2.X; V[0][2] = a3.X;  V[0][3] = 0.f;
    V[1][0] = a1.Y; V[1][1] = a2.Y; V[1][2] = a3.Y;  V[1][3] = 0.f;
    V[2][0] = a1.Z; V[2][1] = a2.Z; V[2][2] = a3.Z;  V[2][3] = 0.f;
    V[3][0] = a1.W; V[3][1] = a2.W; V[3][2] = a3.W;  V[3][3] = 1.f;
}

Matrix4::Matrix4(const Vector4& a1, const Vector4& a2, const Vector4& a3, const Vector4& a4)
{
    V[0][0] = a1.X; V[0][1] = a2.X; V[0][2] = a3.X;  V[0][3] = a4.X;
    V[1][0] = a1.Y; V[1][1] = a2.Y; V[1][2] = a3.Y;  V[1][3] = a4.Y;
    V[2][0] = a1.Z; V[2][1] = a2.Z; V[2][2] = a3.Z;  V[2][3] = a4.Z;
    V[3][0] = a1.W; V[3][1] = a2.W; V[3][2] = a3.W;  V[3][3] = a4.W;
}

Matrix4::Matrix4(const Matrix4& matrix)
{
    SetAllValues(matrix.V[0][0], matrix.V[0][1], matrix.V[0][2], matrix.V[0][3],
        matrix.V[1][0], matrix.V[1][1], matrix.V[1][2], matrix.V[1][3],
        matrix.V[2][0], matrix.V[2][1], matrix.V[2][2], matrix.V[2][3],
        matrix.V[3][0], matrix.V[3][1], matrix.V[3][2], matrix.V[3][3]);
}

Matrix4 Matrix4::operator+(const Matrix4& n) const
{
    return Matrix4(V[0][0] + n.V[0][0], V[0][1] + n.V[0][1], V[0][2] + n.V[0][2], V[0][3] + n.V[0][3],
        V[1][0] + n.V[1][0], V[1][1] + n.V[1][1], V[1][2] + n.V[1][2], V[1][3] + n.V[1][3],
        V[2][0] + n.V[2][0], V[2][1] + n.V[2][1], V[2][2] + n.V[2][2], V[2][3] + n.V[2][3],
        V[3][0] + n.V[3][0], V[3][1] + n.V[3][1], V[3][2] + n.V[3][2], V[3][3] + n.V[3][3]);
}

Matrix4& Matrix4::operator+=(const Matrix4& n)
{
    V[0][0] += n.V[0][0]; V[0][1] += n.V[0][1]; V[0][2] += n.V[0][2]; V[0][3] += n.V[0][3];
    V[1][0] += n.V[1][0]; V[1][1] += n.V[1][1]; V[1][2] += n.V[1][2]; V[1][3] += n.V[1][3];
    V[2][0] += n.V[2][0]; V[2][1] += n.V[2][1]; V[2][2] += n.V[2][2]; V[2][3] += n.V[2][3];
    V[3][0] += n.V[3][0]; V[3][1] += n.V[3][1]; V[3][2] += n.V[3][2]; V[3][3] += n.V[3][3];
    return *this;
}

Matrix4 Matrix4::operator-(const Matrix4& n) const
{
    return Matrix4(V[0][0] - n.V[0][0], V[0][1] - n.V[0][1], V[0][2] - n.V[0][2], V[0][3] - n.V[0][3],
        V[1][0] - n.V[1][0], V[1][1] - n.V[1][1], V[1][2] - n.V[1][2], V[1][3] - n.V[1][3],
        V[2][0] - n.V[2][0], V[2][1] - n.V[2][1], V[2][2] - n.V[2][2], V[2][3] - n.V[2][3],
        V[3][0] - n.V[3][0], V[3][1] - n.V[3][1], V[3][2] - n.V[3][2], V[3][3] - n.V[3][3]);
}

Matrix4& Matrix4::operator-=(const Matrix4& n)
{
    V[0][0] -= n.V[0][0]; V[0][1] -= n.V[0][1]; V[0][2] -= n.V[0][2]; V[0][3] -= n.V[0][3];
    V[1][0] -= n.V[1][0]; V[1][1] -= n.V[1][1]; V[1][2] -= n.V[1][2]; V[1][3] -= n.V[1][3];
    V[2][0] -= n.V[2][0]; V[2][1] -= n.V[2][1]; V[2][2] -= n.V[2][2]; V[2][3] -= n.V[2][3];
    V[3][0] -= n.V[3][0]; V[3][1] -= n.V[3][1]; V[3][2] -= n.V[3][2]; V[3][3] -= n.V[3][3];
    return *this;
}

Matrix4& Matrix4::operator=(const Matrix4& matrix)
{
    if (&matrix != this)
    {
        SetAllValues(matrix.V[0][0], matrix.V[0][1], matrix.V[0][2], matrix.V[0][3],
            matrix.V[1][0], matrix.V[1][1], matrix.V[1][2], matrix.V[1][3],
            matrix.V[2][0], matrix.V[2][1], matrix.V[2][2], matrix.V[2][3],
            matrix.V[3][0], matrix.V[3][1], matrix.V[3][2], matrix.V[3][3]);
    }
    return *this;
}

bool Matrix4::operator==(const Matrix4& n) const
{
    return V[0][0] == n.V[0][0] && V[0][1] == n.V[0][1] && V[0][2] == n.V[0][2] && V[0][3] == n.V[0][3] &&
        V[1][0] == n.V[1][0] && V[1][1] == n.V[1][1] && V[1][2] == n.V[1][2] && V[1][3] == n.V[1][3] &&
        V[2][0] == n.V[2][0] && V[2][1] == n.V[2][1] && V[2][2] == n.V[2][2] && V[2][3] == n.V[2][3] &&
        V[3][0] == n.V[3][0] && V[3][1] == n.V[3][1] && V[3][2] == n.V[3][2] && V[3][3] == n.V[3][3];
}

Matrix4 Matrix4::operator*(const Matrix4& n) const
{
    const Vector4 SrcA0 = M[0];
    const Vector4 SrcA1 = M[1];
    const Vector4 SrcA2 = M[2];
    const Vector4 SrcA3 = M[3];

    const Vector4 SrcB0 = n.M[0];
    const Vector4 SrcB1 = n.M[1];
    const Vector4 SrcB2 = n.M[2];
    const Vector4 SrcB3 = n.M[3];

    Matrix4 Result;
    Result.M[0] = SrcA0 * SrcB0.Data[0] + SrcA1 * SrcB0.Data[1] + SrcA2 * SrcB0.Data[2] + SrcA3 * SrcB0.Data[3];
    Result.M[1] = SrcA0 * SrcB1.Data[0] + SrcA1 * SrcB1.Data[1] + SrcA2 * SrcB1.Data[2] + SrcA3 * SrcB1.Data[3];
    Result.M[2] = SrcA0 * SrcB2.Data[0] + SrcA1 * SrcB2.Data[1] + SrcA2 * SrcB2.Data[2] + SrcA3 * SrcB2.Data[3];
    Result.M[3] = SrcA0 * SrcB3.Data[0] + SrcA1 * SrcB3.Data[1] + SrcA2 * SrcB3.Data[2] + SrcA3 * SrcB3.Data[3];
    return Result;
}

Vector3 Matrix4::operator*(const Vector3& v) const
{
    const Vector3 u = Vector3(V[0][0] * v.X + V[0][1] * v.Y + V[0][2] * v.Z + V[0][3],
        V[1][0] * v.X + V[1][1] * v.Y + V[1][2] * v.Z + V[1][3],
        V[2][0] * v.X + V[2][1] * v.Y + V[2][2] * v.Z + V[2][3]);
    const float w = V[3][0] * v.X + V[3][1] * v.Y + V[3][2] * v.Z + V[3][3];
    return u / w;
}

Vector4 Matrix4::operator*(const Vector4& v) const
{
    Vector4 u = Vector4(V[0][0] * v.X + V[0][1] * v.Y + V[0][2] * v.Z + v.W * V[0][3],
        V[1][0] * v.X + V[1][1] * v.Y + V[1][2] * v.Z + v.W * V[1][3],
        V[2][0] * v.X + V[2][1] * v.Y + V[2][2] * v.Z + v.W * V[2][3],
        V[3][0] * v.X + V[3][1] * v.Y + V[3][2] * v.Z + v.W * V[3][3]);
    return u.W != 0 ? u / u.W : u;
}

Matrix4 Matrix4::operator*(float f) const
{
    return Matrix4(V[0][0] * f, V[0][1] * f, V[0][2] * f, V[0][3] * f,
        V[1][0] * f, V[1][1] * f, V[1][2] * f, V[1][3] * f,
        V[2][0] * f, V[2][1] * f, V[2][2] * f, V[2][3] * f,
        V[3][0] * f, V[3][1] * f, V[3][2] * f, V[3][3] * f);
}

Matrix4& Matrix4::operator*=(float f)
{
    V[0][0] *= f; V[0][1] *= f; V[0][2] *= f;  V[0][3] *= f;
    V[1][0] *= f; V[1][1] *= f; V[1][2] *= f;  V[1][3] *= f;
    V[2][0] *= f; V[2][1] *= f; V[2][2] *= f;  V[2][3] *= f;
    V[3][0] *= f; V[3][1] *= f; V[3][2] *= f;  V[3][3] *= f;
    return *this;
}

Matrix4 Matrix4::operator/(float f) const
{
    return Matrix4(V[0][0] / f, V[0][1] / f, V[0][2] / f, V[0][3] / f,
        V[1][0] / f, V[1][1] / f, V[1][2] / f, V[1][3] / f,
        V[2][0] / f, V[2][1] / f, V[2][2] / f, V[2][3] / f,
        V[3][0] / f, V[3][1] / f, V[3][2] / f, V[3][3] / f);
}

Matrix4& Matrix4::operator/=(float f)
{
    V[0][0] /= f; V[0][1] /= f; V[0][2] /= f;  V[0][3] /= f;
    V[1][0] /= f; V[1][1] /= f; V[1][2] /= f;  V[1][3] /= f;
    V[2][0] /= f; V[2][1] /= f; V[2][2] /= f;  V[2][3] /= f;
    V[3][0] /= f; V[3][1] /= f; V[3][2] /= f;  V[3][3] /= f;
    return *this;
}

Matrix4 Matrix4::operator-() const
{
    return Matrix4(-V[0][0], -V[0][1], -V[0][2], -V[0][3],
        -V[1][0], -V[1][1], -V[1][2], -V[1][3],
        -V[2][0], -V[2][1], -V[2][2], -V[2][3],
        -V[3][0], -V[3][1], -V[3][2], -V[3][3]);
}

Matrix4 Matrix4::Transpose() const
{
    return Matrix4(V[0][0], V[1][0], V[2][0], V[3][0],
        V[0][1], V[1][1], V[2][1], V[3][1],
        V[0][2], V[1][2], V[2][2], V[3][2],
        V[0][3], V[1][3], V[2][3], V[3][3]);
}

Matrix4 Matrix4::Inverse() const
{
    int indxc[4], indxr[4];
    int ipiv[4] = { 0, 0, 0, 0 };
    float minv[4][4];
    float temp;

    for (int s = 0; s < 4; s++)
    {
        for (int t = 0; t < 4; t++)
        {
            minv[s][t] = V[s][t];
        }
    }

    for (int i = 0; i < 4; i++)
    {
        int irow = -1, icol = -1;
        float big = 0.;
        // Choose pivot
        for (int j = 0; j < 4; j++)
        {
            if (ipiv[j] != 1)
            {
                for (int k = 0; k < 4; k++)
                {
                    if (ipiv[k] == 0)
                    {
                        float abs = minv[j][k];
                        if (abs >= big)
                        {
                            big = abs;
                            irow = j;
                            icol = k;
                        }
                    }
                }
            }
        }
        ++ipiv[icol];

        if (irow != icol)
        {
            for (int k = 0; k < 4; ++k)
            {
                temp = minv[irow][k];
                minv[irow][k] = minv[icol][k];
                minv[icol][k] = temp;
            }
        }
        indxr[i] = irow;
        indxc[i] = icol;

        float pivinv = 1.f / minv[icol][icol];
        minv[icol][icol] = 1.f;
        for (int j = 0; j < 4; j++)
        {
            minv[icol][j] *= pivinv;
        }

        for (int j = 0; j < 4; j++)
        {
            if (j != icol)
            {
                float save = minv[j][icol];
                minv[j][icol] = 0;
                for (int k = 0; k < 4; k++)
                {
                    minv[j][k] -= minv[icol][k] * save;
                }
            }
        }
    }

    for (int j = 3; j >= 0; j--)
    {
        if (indxr[j] != indxc[j])
        {
            for (int k = 0; k < 4; k++)
            {
                temp = minv[k][indxr[j]];
                minv[k][indxr[j]] = minv[k][indxc[j]];
                minv[k][indxc[j]] = temp;
            }
        }
    }
    return Matrix4(minv);
}

void Matrix4::SetAllValues(float a1, float a2, float a3, float a4,
    float b1, float b2, float b3, float b4,
    float c1, float c2, float c3, float c4,
    float d1, float d2, float d3, float d4)
{
    V[0][0] = a1; V[0][1] = a2; V[0][2] = a3, V[0][3] = a4;
    V[1][0] = b1; V[1][1] = b2; V[1][2] = b3; V[1][3] = b4;
    V[2][0] = c1; V[2][1] = c2; V[2][2] = c3; V[2][3] = c4;
    V[3][0] = d1; V[3][1] = d2; V[3][2] = d3; V[3][3] = d4;
}

Matrix4& Matrix4::SetToIdentity()
{
    V[0][0] = 1.f; V[0][1] = 0.f; V[0][2] = 0.f;  V[0][3] = 0.f;
    V[1][0] = 0.f; V[1][1] = 1.f; V[1][2] = 0.f;  V[1][3] = 0.f;
    V[2][0] = 0.f; V[2][1] = 0.f; V[2][2] = 1.f;  V[2][3] = 0.f;
    V[3][0] = 0.f; V[3][1] = 0.f; V[3][2] = 0.f;  V[3][3] = 1.f;
    return *this;
}

Matrix4 Matrix4::Translate(const Vector3& v)
{
    Matrix4 result(*this);
    result.M[3] = M[0] * v.Data[0] + M[1] * v.Data[1] + M[2] * v.Data[2] + M[3];
    return result;
}

Matrix4 Matrix4::Scale(const Vector3& v)
{
    Matrix4 result;
    result.M[0] = M[0] * v.Data[0];
    result.M[1] = M[1] * v.Data[1];
    result.M[2] = M[2] * v.Data[2];
    result.M[3] = M[3];
    return result;
}

Matrix4 Matrix4::RotateX(float angle)
{
    return *this * Matrix4(
        1, 0, 0, 0,
        0, Math::Cos(angle), -Math::Sin(angle), 0,
        0, Math::Sin(angle), Math::Cos(angle), 0,
        0, 0, 0, 1
    );
}

Matrix4 Matrix4::RotateY(float angle)
{
    return *this * Matrix4(
        Math::Cos(angle), 0, Math::Sin(angle), 0,
        0, 1, 0, 0,
        -Math::Sin(angle), 0, Math::Cos(angle), 0,
        0, 0, 0, 1
    );
}

Matrix4 Matrix4::RotateZ(float angle)
{
    return *this * Matrix4(
        Math::Cos(angle), -Math::Sin(angle), 0, 0,
        Math::Sin(angle), Math::Cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

Matrix4 Matrix4::Rotate(const Vector3& axis, float angle)
{
    /*float s = Math::Sin(angle);
    float c = Math::Cos(angle);
    float t = 1 - c;
    Vector3 a = axis.Normalized();

    return *this * Matrix4(
        a.X * a.X * t + c, a.X * a.Y * t - a.Z * s, a.X * a.Z * t + a.Y * s, 0,
        a.Y * a.X * t + a.Z * s, a.Y * a.Y * t + c, a.Y * a.Z * t - a.X * s, 0,
        a.Z * a.X * t - a.Y * s, a.Z * a.Y * t + a.X * s, a.Z * a.Z * t + c, 0,
        0, 0, 0, 1
    );*/

    const float a = angle;
    const float c = cos(a);
    const float s = sin(a);

    Vector3 axisNormalized(axis.Normalized());
    Vector3 temp((1.0f - c) * axis);

    Matrix4 rotate;
    rotate.V[0][0] = c + temp.Data[0] * axisNormalized.Data[0];
    rotate.V[0][1] = temp.Data[0] * axisNormalized.Data[1] + s * axisNormalized.Data[2];
    rotate.V[0][2] = temp.Data[0] * axisNormalized.Data[2] - s * axisNormalized.Data[1];

    rotate.V[1][0] = temp.Data[1] * axisNormalized.Data[0] - s * axisNormalized.Data[2];
    rotate.V[1][1] = c + temp.Data[1] * axisNormalized.Data[1];
    rotate.V[1][2] = temp.Data[1] * axisNormalized.Data[2] + s * axisNormalized.Data[0];

    rotate.V[2][0] = temp.Data[2] * axisNormalized.Data[0] + s * axisNormalized.Data[1];
    rotate.V[2][1] = temp.Data[2] * axisNormalized.Data[1] - s * axisNormalized.Data[0];
    rotate.V[2][2] = c + temp.Data[2] * axisNormalized.Data[2];

    Matrix4 result;
    result.M[0] = M[0] * rotate.V[0][0] + M[1] * rotate.V[0][1] + M[2] * rotate.V[0][2];
    result.M[1] = M[0] * rotate.V[1][0] + M[1] * rotate.V[1][1] + M[2] * rotate.V[1][2];
    result.M[2] = M[0] * rotate.V[2][0] + M[1] * rotate.V[2][1] + M[2] * rotate.V[2][2];
    result.M[3] = M[3];
    return result;
}

Matrix4 Matrix4::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
    Matrix4 res;

    res.V[0][0] = zNear * 2.0f / (right - left);
    res.V[1][1] = zNear * 2.0f / (top - bottom);
    res.V[2][0] = (right + left) / (right - left);
    res.V[2][1] = (top + bottom) / (top - bottom);
    res.V[2][2] = (-zFar - zNear) / (zFar - zNear);
    res.V[2][3] = -1;
    res.V[3][2] = -2.0f * zFar * zNear / (zFar - zNear);
    res.V[3][3] = 0;

    return res;
}

Matrix4 Matrix4::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
    GARBAGE_CORE_ASSERT(Math::Abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tanHalfFovy = Math::Tan(fovy / 2.0f);

    Matrix4 result(0.0f);
    result.V[0][0] = 1.0f / (aspect * tanHalfFovy);
    result.V[1][1] = 1.0f / (tanHalfFovy);
    result.V[2][2] = (zFar + zNear) / (zFar - zNear);
    result.V[2][3] = 1.0f;
    result.V[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    return result;
}

Matrix4 Matrix4::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
    Matrix4 result(2.0f / (right - left), 0, 0, 0,
        0, 2.0f / (top - bottom), 0, 0,
        0, 0, -2.0f / (zFar - zNear), 0,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), (-zFar + zNear) / (zFar - zNear), 1.0f);

    return result;
}

Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
    const Vector3 f((center - eye).Normalized());
    const Vector3 s(up.Cross(f).Normalized());
    const Vector3 u(f.Cross(s));

    Matrix4 result(1.0f);
    result.V[0][0] = s.X;
    result.V[1][0] = s.Y;
    result.V[2][0] = s.Z;
    result.V[0][1] = u.X;
    result.V[1][1] = u.Y;
    result.V[2][1] = u.Z;
    result.V[0][2] = f.X;
    result.V[1][2] = f.Y;
    result.V[2][2] = f.Z;
    result.V[3][0] = -s.Dot(eye);
    result.V[3][1] = -u.Dot(eye);
    result.V[3][2] = -f.Dot(eye);
    return result;
}

Vector3 Matrix4::UnProject(const Vector3& vec, const Matrix4& view, const Matrix4& proj, const Rect& viewport)
{
    Matrix4 inv = (proj * view).Inverse();
    Vector3 v(
        (vec.X - viewport.Position.X) * 2.0f / viewport.Size.X - 1.0f,
        (vec.Y - viewport.Position.Y) * 2.0f / viewport.Size.Y - 1.0f,
        2.0f * vec.Z - 1.0f
    );

    Vector3 res = inv * v;
    float w = inv.V[0][3] * v.X + inv.V[1][3] * v.Y + inv.V[2][3] * v.Z + inv.V[3][3];

    return res / w;
}

Vector3 Matrix4::Project(const Vector3& vec, const Matrix4& view, const Matrix4& proj, const Rect& viewport)
{
    Matrix4 trans = proj * view;
    Vector3 v = trans * vec;

    float w = trans.V[0][3] * vec.X + trans.V[1][3] * vec.Y + trans.V[2][3] * vec.Z + trans.V[3][3];
    v = v / w;

    return Vector3(
        viewport.Position.X + viewport.Size.X * (v.X + 1.0f) / 2.0f,
        viewport.Position.Y + viewport.Size.Y * (v.Y + 1.0f) / 2.0f,
        (v.Z + 1.0f) / 2.0f
    );
}