#include "Math/Quaternion.h"

namespace Utils
{

    FORCEINLINE static float ClampAxis(float Angle) noexcept
    {
        Angle = Math::FMod(Angle, 360.f);

        if (Angle < 0.f)
        {
            // shift to [0,360) range
            Angle += 360.f;
        }

        return Angle;
    }

    FORCEINLINE static float NormalizeAxis(float Angle)
    {
        Angle = ClampAxis(Angle);

        if (Angle > 180.f)
        {
            // shift to (-180,180]
            Angle -= 360.f;
        }

        return Angle;
    }

}

Quaternion Quaternion::Identity = Quaternion();

Quaternion::Quaternion(const Vector3& eulerAngles)
{
    float cy = Math::Cos(eulerAngles.Z * 0.5f);
    float sy = Math::Sin(eulerAngles.Z * 0.5f);
    float cp = Math::Cos(eulerAngles.Y * 0.5f);
    float sp = Math::Sin(eulerAngles.Y * 0.5f);
    float cr = Math::Cos(eulerAngles.X * 0.5f);
    float sr = Math::Sin(eulerAngles.X * 0.5f);

    W = cr * cp * cy + sr * sp * sy;
    X = sr * cp * cy - cr * sp * sy;
    Y = cr * sp * cy + sr * cp * sy;
    Z = cr * cp * sy - sr * sp * cy;
}

Quaternion Quaternion::Normalized() const
{
    float length = GetLength();
    return Quaternion(X / length, Y / length, Z / length, W / length);
}

Vector3 Quaternion::RotateVector(const Vector3& v) const
{
    const Vector3 Q(X, Y, Z);
    const Vector3 T = Q.Cross(v) * 2.f;
    const Vector3 result = v + (T * W) + Q.Cross(T);
    return result;
}

// Fully and clearly stea...I mean, inspired from UE source
Vector3 Quaternion::GetEulerAngles() const
{
    Vector3 euler(0.0f);

    const float singularityTest = Z * X - W * Y;
    const float yawY = 2.f * (W * Z + X * Y);
    const float yawX = (1.f - 2.f * (Math::Square(Y) + Math::Square(Z)));

    // reference 
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

    // this value was found from experience, the above websites recommend different values
    // but that isn't the case for us, so I went through different testing, and finally found the case 
    // where both of world lives happily. 
    const float SINGULARITY_THRESHOLD = 0.4999995f;

    if (singularityTest < -SINGULARITY_THRESHOLD)
    {
        euler.Y = -90.f;
        euler.Z = Math::Atan2(yawY, yawX) * Math::Rad2Deg;
        euler.X = Utils::NormalizeAxis(-euler.Z - (2.f * Math::Atan2(X, W) * Math::Rad2Deg));
    }
    else if (singularityTest > SINGULARITY_THRESHOLD)
    {
        euler.Y = 90.f;
        euler.Z = Math::Atan2(yawY, yawX) * Math::Rad2Deg;
        euler.X = Utils::NormalizeAxis(euler.Z - (2.f * Math::Atan2(X, W) * Math::Rad2Deg));
    }
    else
    {
        euler.Y = Math::Asin(2.f * (singularityTest)) * Math::Rad2Deg;
        euler.Z = Math::Atan2(yawY, yawX) * Math::Rad2Deg;
        euler.X = Math::Atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * (Math::Square(X) + Math::Square(Y)))) * Math::Rad2Deg;
    }

    return euler;
}

Quaternion Quaternion::LookAt(Vector3 from, Vector3 to)
{
    Vector3 forwardVector = (to - from).Normalized();

    float dot = forwardVector.Dot(Vector3::Forward);

    if (Math::Abs(dot - (-1.0f)) < 0.000001f)
    {
        return Quaternion(Vector3::Up.X, Vector3::Up.Y, Vector3::Up.Z, Math::Pi);
    }
    if (Math::Abs(dot - (1.0f)) < 0.000001f)
    {
        return Quaternion::Identity;
    }

    float rotAngle = (float)Math::Acos(dot);
    Vector3 rotAxis = Vector3::Forward.Cross(forwardVector).Normalized();
    return CreateFromAxisAngle(rotAxis, rotAngle);
}

Quaternion Quaternion::CreateFromTwoVectors(Vector3 unitVector0, Vector3 unitVector1)
{
    float cosHalfAngleX2, recipCosHalfAngleX2;
    cosHalfAngleX2 = sqrt((2.0f * (1.0f + unitVector0.Dot(unitVector1))));
    recipCosHalfAngleX2 = (1.0f / cosHalfAngleX2);

    return Quaternion((unitVector0.Cross(unitVector1) * recipCosHalfAngleX2), (cosHalfAngleX2 * 0.5f));
}

Quaternion Quaternion::CreateFromAxisAngle(Vector3 axis, float angle)
{
    float halfAngle = angle * .5f;
    float s = Math::Sin(halfAngle);
    return Quaternion(axis.X * s, axis.Y * s, axis.Z * s, Math::Cos(halfAngle));
}