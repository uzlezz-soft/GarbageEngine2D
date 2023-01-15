#include "Math/Vector4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

GARBAGE_API Vector4 Vector4::Forward = Vector4(1, 0, 0, 0);
GARBAGE_API Vector4 Vector4::Backward = Vector4(-1, 0, 0, 0);
GARBAGE_API Vector4 Vector4::Right = Vector4(0, 1, 0, 0);
GARBAGE_API Vector4 Vector4::Left = Vector4(0, -1, 0, 0);
GARBAGE_API Vector4 Vector4::Up = Vector4(0, 0, 1, 0);
GARBAGE_API Vector4 Vector4::Down = Vector4(0, 0, -1, 0);
GARBAGE_API Vector4 Vector4::One = Vector4(1, 1, 1, 1);
GARBAGE_API Vector4 Vector4::Zero = Vector4(0, 0, 0, 0);

Vector4::Vector4(const Vector2& v, float z, float w) : X(v.X), Y(v.Y), Z(z), W(w) {}

Vector4::Vector4(const Vector3& v, float w) : X(v.X), Y(v.Y), Z(v.Z), W(w) {}

Vector4& Vector4::operator+=(const Vector4& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	W += v.W;
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	W -= v.W;
	return *this;
}

Vector4& Vector4::operator*=(const Vector4& v)
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	W *= v.W;
	return *this;
}

Vector4& Vector4::operator/=(const Vector4& v)
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;
	W /= v.W;
	return *this;
}

Vector4& Vector4::operator+=(const Vector2& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

Vector4& Vector4::operator-=(const Vector2& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

Vector4& Vector4::operator*=(const Vector2& v)
{
	X *= v.X;
	Y *= v.Y;
	return *this;
}

Vector4& Vector4::operator/=(const Vector2& v)
{
	X /= v.X;
	Y /= v.Y;
	return *this;
}

Vector4& Vector4::operator+=(const Vector3& v)
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	return *this;
}

Vector4& Vector4::operator-=(const Vector3& v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	return *this;
}

Vector4& Vector4::operator*=(const Vector3& v)
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	return *this;
}

Vector4& Vector4::operator/=(const Vector3& v)
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;
	return *this;
}

const Vector4 Vector4::operator+(const Vector4& v) const
{
	return Vector4(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
}

const Vector4 Vector4::operator-(const Vector4& v) const
{
	return Vector4(X - v.X, Y - v.Y, Z - v.Z, W - v.W);
}

const Vector4 Vector4::operator*(const Vector4& v) const
{
	return Vector4(X * v.X, Y * v.Y, Z * v.Z, W * v.W);
}

const Vector4 Vector4::operator/(const Vector4& v) const
{
	return Vector4(X / v.X, Y / v.Y, Z / v.Z, W / v.W);
}

const Vector4 Vector4::operator+(const Vector2& v) const
{
	return Vector4(X + v.X, Y + v.Y, Z, W);
}

const Vector4 Vector4::operator-(const Vector2& v) const
{
	return Vector4(X - v.X, Y - v.Y, Z, W);
}

const Vector4 Vector4::operator*(const Vector2& v) const
{
	return Vector4(X * v.X, Y * v.Y, Z, W);
}

const Vector4 Vector4::operator/(const Vector2& v) const
{
	return Vector4(X / v.X, Y / v.Y, Z, W);
}

const Vector4 Vector4::operator+(const Vector3& v) const
{
	return Vector4(X + v.X, Y + v.Y, Z + v.Z, W);
}

const Vector4 Vector4::operator-(const Vector3& v) const
{
	return Vector4(X - v.X, Y - v.Y, Z - v.Z, -W);
}

const Vector4 Vector4::operator*(const Vector3& v) const
{
	return Vector4(X * v.X, Y * v.Y, Z * v.Z, W);
}

const Vector4 Vector4::operator/(const Vector3& v) const
{
	return Vector4(X / v.X, Y / v.Y, Z / v.Z, W);
}

Vector4& Vector4::operator*=(float n)
{
	X *= n;
	Y *= n;
	Z *= n;
	W *= n;
	return *this;
}

Vector4& Vector4::operator/=(float n)
{
	X /= n;
	Y /= n;
	Z /= n;
	W /= n;
	return *this;
}

void Vector4::Normalize()
{
	*this = Normalized();
}

void Vector4::Project(const Vector4& other)
{
	*this = Projected(other);
}

void Vector4::ProjectOnNormal(const Vector4& normal)
{
	*this = ProjectedOnNormal(normal);
}

Vector4 Vector4::Lerp(const Vector4& dest, float t) const
{
	return Vector4(Math::Lerp(X, dest.X, t), Math::Lerp(Y, dest.Y, t), Math::Lerp(Z, dest.Z, t), Math::Lerp(W, dest.W, t));
}

Vector4 Vector4::Slerp(const Vector4& dest, float t) const
{
	const float dot = Math::Clamp(Dot(dest), -1.0f, 1.0f);
	const float theta = Math::Acos(dot) * t;
	const Vector4 relative = (dest - (*this * dot)).Normalized();

	return ((*this * Math::Cos(theta)) + (relative * Math::Sin(theta)));
}

Vector4 Vector4::Nlerp(const Vector4& dest, float t) const
{
	return Lerp(dest, t).Normalized();
}

float Vector4::Dot(const Vector4& v) const
{
	return X * v.X + Y * v.Y + Z * v.Z + W * v.W;
}

float Vector4::Angle(const Vector4& v) const
{
	return Math::Acos(Dot(v) / GetLength() / v.GetLength());
}

float Vector4::GetLengthSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

float Vector4::GetLength() const
{
	return Math::Sqrt(GetLengthSquared());
}

float Vector4::DistanceSquared(const Vector4& v) const
{
	return (*this - v).GetLengthSquared();
}

float Vector4::Distance(const Vector4& v) const
{
	return (*this - v).GetLength();
}

Vector4 Vector4::Abs() const
{
	return Vector4(Math::Abs(X), Math::Abs(Y), Math::Abs(Z), Math::Abs(W));
}

Vector4 Vector4::Normalized() const
{
	return *this / GetLength();
}

Vector4 Vector4::Projected(const Vector4& other) const
{
	return other * (Dot(other) / other.Dot(other));
}

Vector4 Vector4::ProjectedOnNormal(const Vector4& normal) const
{
	return normal * Dot(normal);
}

Vector4 Vector4::Floor() const
{
	return Vector4((float)Math::Floor(X), (float)Math::Floor(Y), (float)Math::Floor(Z), (float)Math::Floor(W));
}

Vector4 Vector4::Ceil() const
{
	return Vector4((float)Math::Ceil(X), (float)Math::Ceil(Y), (float)Math::Ceil(Z), (float)Math::Ceil(W));
}

Vector4 Vector4::Round() const
{
	return Vector4((float)Math::Round(X), (float)Math::Round(Y), (float)Math::Round(Z), (float)Math::Round(W));
}

Vector4 Vector4::Min(const Vector4& other) const
{
	return Vector4(Math::Min(X, other.X), Math::Min(Y, other.Y), Math::Min(Z, other.Z), Math::Min(W, other.W));
}

Vector4 Vector4::Max(const Vector4& other) const
{
	return Vector4(Math::Max(X, other.X), Math::Max(Y, other.Y), Math::Max(Z, other.Z), Math::Max(W, other.W));
}

Vector4 Vector4::Clamp(const Vector4& min, const Vector4& max) const
{
	return Vector4(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y), Math::Clamp(Z, min.Z, max.Z), Math::Clamp(W, min.W, max.W));
}

GARBAGE_API Vector4 operator*(const Vector4& v, float n)
{
	return Vector4(v.X * n, v.Y * n, v.Z * n, v.W * n);
}

GARBAGE_API Vector4 operator*(float n, const Vector4& v)
{
	return operator*(v, n);
}

GARBAGE_API Vector4 operator/(const Vector4& v, float n)
{
	return (v.X / n, v.Y / n, v.Z / n, v.W / n);
}

GARBAGE_API Vector4 operator/(float n, const Vector4& v)
{
	return operator/(v, n);
}
