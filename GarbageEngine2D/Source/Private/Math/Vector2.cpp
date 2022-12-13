#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

GARBAGE_API Vector2 Vector2::Right = Vector2(1, 0);
GARBAGE_API Vector2 Vector2::Left = Vector2(-1, 0);
GARBAGE_API Vector2 Vector2::Up = Vector2(0, 1);
GARBAGE_API Vector2 Vector2::Down = Vector2(0, -1);
GARBAGE_API Vector2 Vector2::Zero = Vector2(0, 0);
GARBAGE_API Vector2 Vector2::One = Vector2(1, 1);

Vector2::Vector2(const Vector3& v) : X(v.X), Y(v.Y) {}

Vector2::Vector2(const Vector4& v) : X(v.X), Y(v.Y) {}

Vector2& Vector2::operator+=(const Vector2& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

Vector2& Vector2::operator*=(const Vector2& v)
{
	X *= v.X;
	Y *= v.Y;
	return *this;
}

Vector2& Vector2::operator/=(const Vector2& v)
{
	X /= v.X;
	Y /= v.Y;
	return *this;
}

Vector2& Vector2::operator+=(const Vector3& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector3& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

Vector2& Vector2::operator*=(const Vector3& v)
{
	X *= v.X;
	Y *= v.Y;
	return *this;
}

Vector2& Vector2::operator/=(const Vector3& v)
{
	X /= v.X;
	Y /= v.Y;
	return *this;
}

Vector2& Vector2::operator+=(const Vector4& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector4& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

Vector2& Vector2::operator*=(const Vector4& v)
{
	X *= v.X;
	Y *= v.Y;
	return *this;
}

Vector2& Vector2::operator/=(const Vector4& v)
{
	X /= v.X;
	Y /= v.Y;
	return *this;
}

const Vector2 Vector2::operator+(const Vector2& v) const
{
	return Vector2(X + v.X, Y + v.Y);
}

const Vector2 Vector2::operator-(const Vector2& v) const
{
	return Vector2(X - v.X, Y - v.Y);
}

const Vector2 Vector2::operator*(const Vector2& v) const
{
	return Vector2(X * v.X, Y * v.Y);
}

const Vector2 Vector2::operator/(const Vector2& v) const
{
	return Vector2(X / v.X, Y / v.Y);
}

const Vector3 Vector2::operator+(const Vector3& v) const
{
	return Vector3(X + v.X, Y + v.Y, v.Z);
}

const Vector3 Vector2::operator-(const Vector3& v) const
{
	return Vector3(X - v.X, Y - v.Y, -v.Z);
}

const Vector3 Vector2::operator*(const Vector3& v) const
{
	return Vector3(X * v.X, Y * v.Y, v.Z);
}

const Vector3 Vector2::operator/(const Vector3& v) const
{
	return Vector3(X / v.X, Y / v.Y, v.Z);
}

const Vector4 Vector2::operator+(const Vector4& v) const
{
	return Vector4(X + v.X, Y + v.Y, v.Z, v.W);
}

const Vector4 Vector2::operator-(const Vector4& v) const
{
	return Vector4(X - v.X, Y - v.Y, -v.Z, -v.W);
}

const Vector4 Vector2::operator*(const Vector4& v) const
{
	return Vector4(X * v.X, Y * v.Y, v.Z, v.W);
}

const Vector4 Vector2::operator/(const Vector4& v) const
{
	return Vector4(X / v.X, Y / v.Y, v.Z, v.W);
}

Vector2& Vector2::operator*=(float n)
{
	X *= n;
	Y *= n;
	return *this;
}

Vector2& Vector2::operator/=(float n)
{
	X /= n;
	Y /= n;
	return *this;
}

void Vector2::Normalize()
{
	*this = Normalized();
}

void Vector2::Project(const Vector2& other)
{
	*this = Projected(other);
}

void Vector2::ProjectOnNormal(const Vector2& normal)
{
	*this = ProjectedOnNormal(normal);
}

Vector2 Vector2::Lerp(const Vector2& dest, float t) const
{
	return Vector2(Math::Lerp(X, dest.X, t), Math::Lerp(Y, dest.Y, t));
}

Vector2 Vector2::Slerp(const Vector2& dest, float t) const
{
	const float dot = Math::Clamp(Dot(dest), -1.0f, 1.0f);
	const float theta = Math::Acos(dot) * t;
	const Vector2 relative = dest - (*this * dot);

	return *this * Math::Cos(theta) + ((dest - (*this * dot)).Normalized() * Math::Sin(theta));
}

Vector2 Vector2::Nlerp(const Vector2& dest, float t) const
{
	return Lerp(dest, t).Normalized();
}

float Vector2::Dot(const Vector2& v) const
{
	return X * v.X + Y * v.Y;
}

float Vector2::Angle(const Vector2& v) const
{
	return Math::Acos(Dot(v) / GetLength() / v.GetLength());
}

float Vector2::GetLengthSquared() const
{
	return X * X + Y * Y;
}

float Vector2::GetLength() const
{
	return Math::Sqrt(GetLengthSquared());
}

float Vector2::DistanceSquared(const Vector2& v) const
{
	return (*this - v).GetLengthSquared();
}

float Vector2::Distance(const Vector2& v) const
{
	return (*this - v).GetLength();
}

Vector2 Vector2::Abs() const
{
	return Vector2(Math::Abs(X), Math::Abs(Y));
}

Vector2 Vector2::Normalized() const
{
	return *this / GetLength();
}

Vector2 Vector2::Projected(const Vector2& other) const
{
	return other * (Dot(other) / other.Dot(other));
}

Vector2 Vector2::ProjectedOnNormal(const Vector2& normal) const
{
	return normal * Dot(normal);
}

Vector2 Vector2::Floor() const
{
	return Vector2((float)Math::Floor(X), (float)Math::Floor(Y));
}

Vector2 Vector2::Ceil() const
{
	return Vector2((float)Math::Ceil(X), (float)Math::Ceil(Y));
}

Vector2 Vector2::Round() const
{
	return Vector2((float)Math::Round(X), (float)Math::Round(Y));
}

Vector2 Vector2::Min(const Vector2& other) const
{
	return Vector2(Math::Min(X, other.X), Math::Min(Y, other.Y));
}

Vector2 Vector2::Max(const Vector2& other) const
{
	return Vector2(Math::Max(X, other.X), Math::Max(Y, other.Y));
}

Vector2 Vector2::Clamp(const Vector2& min, const Vector2& max) const
{
	return Vector2(Math::Clamp(X, min.X, max.X), Math::Clamp(Y, min.Y, max.Y));
}

Vector2 operator*(const Vector2& v, float n)
{
	return Vector2(v.X * n, v.Y * n);
}

Vector2 operator*(float n, const Vector2& v)
{
	return Vector2(v.X * n, v.Y * n);
}

Vector2 operator/(const Vector2& v, float n)
{
	return Vector2(v.X / n, v.Y / n);
}

Vector2 operator/(float n, const Vector2& v)
{
	return Vector2(v.X / n, v.Y / n);
}

Vector2 Vector2::MoveTowards(const Vector2& other, float deltaDistance) const
{
	if (Distance(other) < deltaDistance) return other;

	Vector2 result = *this + (Vector2(other.X - X, other.Y - Y).Normalized() * deltaDistance);
	if (result.Distance(other) < deltaDistance) return other;

	return result;
}