#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

GARBAGE_API Vector3 Vector3::Forward = Vector3(1, 0, 0);
GARBAGE_API Vector3 Vector3::Backward = Vector3(-1, 0, 0);
GARBAGE_API Vector3 Vector3::Right = Vector3(0, 1, 0);
GARBAGE_API Vector3 Vector3::Left = Vector3(0, -1, 0);
GARBAGE_API Vector3 Vector3::Up = Vector3(0, 0, 1);
GARBAGE_API Vector3 Vector3::Down = Vector3(0, 0, -1);
GARBAGE_API Vector3 Vector3::One = Vector3(1, 1, 1);
GARBAGE_API Vector3 Vector3::Zero = Vector3(0, 0, 0);

Vector3::Vector3(const Vector2& v, float z) : X(v.X), Y(v.Y), Z(z) {}

Vector3::Vector3(const Vector4& v) : X(v.X), Y(v.Y), Z(v.Z) {}

Vector3& Vector3::operator+=(const Vector3& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& v)
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	return *this;
}

Vector3& Vector3::operator/=(const Vector3& v)
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;
	return *this;
}

Vector3& Vector3::operator+=(const Vector2& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

Vector3& Vector3::operator-=(const Vector2& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

Vector3& Vector3::operator*=(const Vector2& v)
{
	X *= v.X;
	Y *= v.Y;
	return *this;
}

Vector3& Vector3::operator/=(const Vector2& v)
{
	X /= v.X;
	Y /= v.Y;
	return *this;
}

Vector3& Vector3::operator+=(const Vector4& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector4& v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	return *this;
}

Vector3& Vector3::operator*=(const Vector4& v)
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	return *this;
}

Vector3& Vector3::operator/=(const Vector4& v)
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;
	return *this;
}

const Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(X + v.X, Y + v.Y, Z + v.Z);
}

const Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(X - v.X, Y - v.Y, Z - v.Z);
}

const Vector3 Vector3::operator*(const Vector3& v) const
{
	return Vector3(X * v.X, Y * v.Y, Z * v.Z);
}

const Vector3 Vector3::operator/(const Vector3& v) const
{
	return Vector3(X / v.X, Y / v.Y, Z / v.Z);
}

const Vector3 Vector3::operator+(const Vector2& v) const
{
	return Vector3(X + v.X, Y + v.Y, Z);
}

const Vector3 Vector3::operator-(const Vector2& v) const
{
	return Vector3(X - v.X, Y - v.Y, Z);
}

const Vector3 Vector3::operator*(const Vector2& v) const
{
	return Vector3(X * v.X, Y * v.Y, Z);
}

const Vector3 Vector3::operator/(const Vector2& v) const
{
	return Vector3(X / v.X, Y / v.Y, Z);
}

const Vector4 Vector3::operator+(const Vector4& v) const
{
	return Vector4(X + v.X, Y + v.Y, Z + v.Z, v.W);
}

const Vector4 Vector3::operator-(const Vector4& v) const
{
	return Vector4(X - v.X, Y - v.Y, Z - v.Z, -v.W);
}

const Vector4 Vector3::operator*(const Vector4& v) const
{
	return Vector4(X * v.X, Y * v.Y, Z * v.Z, v.W);
}

const Vector4 Vector3::operator/(const Vector4& v) const
{
	return Vector4(X / v.X, Y / v.Y, Z / v.Z, v.W);
}

Vector3& Vector3::operator*=(float n)
{
	X *= n;
	Y *= n;
	Z *= n;
	return *this;
}

Vector3& Vector3::operator/=(float n)
{
	X /= n;
	Y /= n;
	Z /= n;
	return *this;
}

void Vector3::Normalize()
{
	*this = Normalized();
}

void Vector3::Project(const Vector3& other)
{
	*this = Projected(other);
}

void Vector3::ProjectOnNormal(const Vector3& normal)
{
	*this = ProjectedOnNormal(normal);
}

Vector3 Vector3::Lerp(const Vector3& dest, float t) const
{
	return Vector3(Math::Lerp(X, dest.X, t), Math::Lerp(Y, dest.Y, t), Math::Lerp(Z, dest.Z, t));
}

Vector3 Vector3::Slerp(const Vector3& dest, float t) const
{
	const float dot = Math::Clamp(Dot(dest), -1.0f, 1.0f);
	const float theta = Math::Acos(dot) * t;
	const Vector3 relative = (dest - (*this * dot)).Normalized();

	return ((*this * Math::Cos(theta)) + (relative * Math::Sin(theta)));
}

Vector3 Vector3::Nlerp(const Vector3& dest, float t) const
{
	return Lerp(dest, t).Normalized();
}

float Vector3::Dot(const Vector3& v) const
{
	return X * v.X + Y * v.Y + Z * v.Z;
}

Vector3 Vector3::Cross(const Vector3& v) const
{
	return Vector3(Y * v.Z - Z * v.Y, Z * v.X - X * v.Z, X * v.Y - Y * v.X);
}

float Vector3::Angle(const Vector3& v) const
{
	return Math::Acos(Dot(v) / GetLength() / v.GetLength());
}

float Vector3::GetLengthSquared() const
{
	return X * X + Y * Y + Z * Z;
}

float Vector3::GetLength() const
{
	return Math::Sqrt(GetLengthSquared());
}

float Vector3::DistanceSquared(const Vector3& v) const
{
	return (*this - v).GetLengthSquared();
}

float Vector3::Distance(const Vector3& v) const
{
	return (*this - v).GetLength();
}

Vector3 Vector3::Abs() const
{
	return Vector3(Math::Abs(X), Math::Abs(Y), Math::Abs(Z));
}

Vector3 Vector3::Normalized() const
{
	return *this / GetLength();
}

Vector3 Vector3::Projected(const Vector3& other) const
{
	return other * (Dot(other) / other.Dot(other));
}

Vector3 Vector3::ProjectedOnNormal(const Vector3& normal) const
{
	return normal * Dot(normal);
}

Vector3 Vector3::Floor() const
{
	return Vector3((float)Math::Floor(X), (float)Math::Floor(Y), (float)Math::Floor(Z));
}

Vector3 Vector3::Ceil() const
{
	return Vector3((float)Math::Ceil(X), (float)Math::Ceil(Y), (float)Math::Ceil(Z));
}

Vector3 Vector3::Round() const
{
	return Vector3((float)Math::Round(X), (float)Math::Round(Y), (float)Math::Round(Z));
}

Vector3 Vector3::Min(const Vector3& other) const
{
	return Vector3(Math::Min(X, other.X), Math::Min(Y, other.Y), Math::Min(Z, other.Z));
}

Vector3 Vector3::Max(const Vector3& other) const
{
	return Vector3(Math::Max(X, other.X), Math::Max(Y, other.Y), Math::Max(Z, other.Z));
}

Vector3 Vector3::Clamp(const Vector3& min, const Vector3& max) const
{
	return Vector3(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y), Math::Clamp(Z, min.Z, max.Z));
}

GARBAGE_API Vector3 operator*(const Vector3& v, float n)
{
	return Vector3(v.X * n, v.Y * n, v.Z * n);
}

GARBAGE_API Vector3 operator*(float n, const Vector3& v)
{
	return operator*(v, n);
}

GARBAGE_API Vector3 operator/(const Vector3& v, float n)
{
	return Vector3(v.X / n, v.Y / n, v.Z / n);
}

GARBAGE_API Vector3 operator/(float n, const Vector3& v)
{
	return operator/(v, n);
}

Vector3 Vector3::MoveTowards(const Vector3& other, float deltaDistance) const
{
	if (Distance(other) < deltaDistance) return other;

	Vector3 result = *this + (Vector3(other.X - X, other.Y - Y, other.Z - Z).Normalized() * deltaDistance);
	if (result.Distance(other) < deltaDistance) return other;

	return result;
}