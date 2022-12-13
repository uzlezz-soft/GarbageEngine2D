#pragma once

#include "Core/Base.h"
#include "Math/Math.h"

class Vector2;
class Vector4;

class GARBAGE_API Vector3 final
{
public:

	// (1, 0, 0)
	static Vector3 Forward;
	// (-1, 0, 0)
	static Vector3 Backward;
	// (0, 1, 0)
	static Vector3 Right;
	// (0, -1, 0)
	static Vector3 Left;
	// (0, 0, 1)
	static Vector3 Up;
	// (0, 0, -1)
	static Vector3 Down;
	// (0, 0, 0)
	static Vector3 Zero;
	// (1, 1, 1)
	static Vector3 One;

	union
	{
		struct { float X, Y, Z; };
		float Data[3];
	};

	Vector3() : X(0), Y(0), Z(0) {}

	Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}
	Vector3(const Vector2& v, float z = 0.0f);
	Vector3(const Vector4& v);
	Vector3(float xyz) : X(xyz), Y(xyz), Z(xyz) {}
	Vector3(const Vector3& vector) : X(vector.X), Y(vector.Y), Z(vector.Z) {}

	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(const Vector3& v);
	Vector3& operator/=(const Vector3& v);
	Vector3& operator+=(const Vector2& v);
	Vector3& operator-=(const Vector2& v);
	Vector3& operator*=(const Vector2& v);
	Vector3& operator/=(const Vector2& v);
	Vector3& operator+=(const Vector4& v);
	Vector3& operator-=(const Vector4& v);
	Vector3& operator*=(const Vector4& v);
	Vector3& operator/=(const Vector4& v);
	const Vector3 operator+(const Vector3& v) const;
	const Vector3 operator-(const Vector3& v) const;
	const Vector3 operator*(const Vector3& v) const;
	const Vector3 operator/(const Vector3& v) const;
	const Vector3 operator+(const Vector2& v) const;
	const Vector3 operator-(const Vector2& v) const;
	const Vector3 operator*(const Vector2& v) const;
	const Vector3 operator/(const Vector2& v) const;
	const Vector4 operator+(const Vector4& v) const;
	const Vector4 operator-(const Vector4& v) const;
	const Vector4 operator*(const Vector4& v) const;
	const Vector4 operator/(const Vector4& v) const;

	Vector3& operator*=(float n);
	Vector3& operator/=(float n);
	GARBAGE_API friend Vector3 operator*(const Vector3& v, float n);
	GARBAGE_API friend Vector3 operator*(float n, const Vector3& v);
	GARBAGE_API friend Vector3 operator/(const Vector3& v, float n);
	GARBAGE_API friend Vector3 operator/(float n, const Vector3& v);

	void Normalize();
	void Project(const Vector3& other);
	void ProjectOnNormal(const Vector3& normal);

	Vector3 Lerp(const Vector3& dest, float t) const;
	Vector3 Slerp(const Vector3& dest, float t) const;
	Vector3 Nlerp(const Vector3& dest, float t) const;
	Vector3 MoveTowards(const Vector3& other, float deltaDistance) const;

	float Dot(const Vector3& v) const;
	Vector3 Cross(const Vector3& v) const;
	float Angle(const Vector3& v) const;
	float GetLengthSquared() const;
	float GetLength() const;
	float DistanceSquared(const Vector3& v) const;
	float Distance(const Vector3& v) const;

	Vector3 Abs() const;
	Vector3 Normalized() const;
	Vector3 Projected(const Vector3& other) const;
	Vector3 ProjectedOnNormal(const Vector3& normal) const;

	Vector3 Floor() const;
	Vector3 Ceil() const;
	Vector3 Round() const;

	Vector3 Min(const Vector3& other) const;
	Vector3 Max(const Vector3& other) const;
	Vector3 Clamp(const Vector3& min, const Vector3& max) const;

};