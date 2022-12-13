#pragma once

#include "Core/Base.h"
#include "Math/Math.h"

class Vector3;
class Vector4;

class GARBAGE_API GARBAGE_ALIGN(8) Vector2 final
{
public:

	// (1, 0)
	static Vector2 Right;
	// (-1, 0)
	static Vector2 Left;
	// (0, 1)
	static Vector2 Up;
	// (0, -1)
	static Vector2 Down;
	// (0, 0)
	static Vector2 Zero;
	// (1, 1)
	static Vector2 One;

	union
	{
		struct { float X, Y; };
		float Data[2];
	};

	Vector2() : X(0), Y(0) {}

	Vector2(float x, float y) : X(x), Y(y) {}
	Vector2(const Vector3& v);
	Vector2(const Vector4& v);
	Vector2(float xy) : X(xy), Y(xy) {}
	Vector2(const Vector2& vector) : X(vector.X), Y(vector.Y) {}

	Vector2& operator+=(const Vector2& v);
	Vector2& operator-=(const Vector2& v);
	Vector2& operator*=(const Vector2& v);
	Vector2& operator/=(const Vector2& v);
	Vector2& operator+=(const Vector3& v);
	Vector2& operator-=(const Vector3& v);
	Vector2& operator*=(const Vector3& v);
	Vector2& operator/=(const Vector3& v);
	Vector2& operator+=(const Vector4& v);
	Vector2& operator-=(const Vector4& v);
	Vector2& operator*=(const Vector4& v);
	Vector2& operator/=(const Vector4& v);
	const Vector2 operator+(const Vector2& v) const;
	const Vector2 operator-(const Vector2& v) const;
	const Vector2 operator*(const Vector2& v) const;
	const Vector2 operator/(const Vector2& v) const;
	const Vector3 operator+(const Vector3& v) const;
	const Vector3 operator-(const Vector3& v) const;
	const Vector3 operator*(const Vector3& v) const;
	const Vector3 operator/(const Vector3& v) const;
	const Vector4 operator+(const Vector4& v) const;
	const Vector4 operator-(const Vector4& v) const;
	const Vector4 operator*(const Vector4& v) const;
	const Vector4 operator/(const Vector4& v) const;

	Vector2& operator*=(float n);
	Vector2& operator/=(float n);
	friend Vector2 operator*(const Vector2& v, float n);
	friend Vector2 operator*(float n, const Vector2& v);
	friend Vector2 operator/(const Vector2& v, float n);
	friend Vector2 operator/(float n, const Vector2& v);

	void Normalize();
	void Project(const Vector2& other);
	void ProjectOnNormal(const Vector2& normal);

	Vector2 Lerp(const Vector2& dest, float t) const;
	Vector2 Slerp(const Vector2& dest, float t) const;
	Vector2 Nlerp(const Vector2& dest, float t) const;
	Vector2 MoveTowards(const Vector2& other, float deltaDistance) const;

	float Dot(const Vector2& v) const;
	float Angle(const Vector2& v) const;
	float GetLengthSquared() const;
	float GetLength() const;
	float DistanceSquared(const Vector2& v) const;
	float Distance(const Vector2& v) const;

	Vector2 Abs() const;
	Vector2 Normalized() const;
	Vector2 Projected(const Vector2& other) const;
	Vector2 ProjectedOnNormal(const Vector2& normal) const;

	Vector2 Floor() const;
	Vector2 Ceil() const;
	Vector2 Round() const;

	Vector2 Min(const Vector2& other) const;
	Vector2 Max(const Vector2& other) const;
	Vector2 Clamp(const Vector2& min, const Vector2& max) const;

};