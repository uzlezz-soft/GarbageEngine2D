#pragma once

#include "Core/Base.h"
#include "Math/Math.h"

class Vector2;
class Vector3;

class GARBAGE_API GARBAGE_ALIGN(16) Vector4 final
{
public:

	// (1, 0, 0, 0)
	static Vector4 Forward;
	// (-1, 0, 0, 0)
	static Vector4 Backward;
	// (0, 1, 0, 0)
	static Vector4 Right;
	// (0,-1, 0, 0)
	static Vector4 Left;
	// (0, 0, 1, 0)
	static Vector4 Up;
	// (0, 0, -1, 0)
	static Vector4 Down;
	// (0, 0, 0, 0)
	static Vector4 Zero;
	// (1, 1, 1, 1)
	static Vector4 One;

	union
	{
		struct { float X, Y, Z, W; };
		float Data[4];
	};

	Vector4() : X(0), Y(0), Z(0) {}

	Vector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
	Vector4(const Vector2& v, float z = 0.0f, float w = 0.0f);
	Vector4(const Vector3& v, float w = 0.0f);
	Vector4(float xyzw) : X(xyzw), Y(xyzw), Z(xyzw), W(xyzw) {}
	Vector4(const Vector4& vector) : X(vector.X), Y(vector.Y), Z(vector.Z), W(vector.W) {}

	Vector4& operator+=(const Vector4& v);
	Vector4& operator-=(const Vector4& v);
	Vector4& operator*=(const Vector4& v);
	Vector4& operator/=(const Vector4& v);
	Vector4& operator+=(const Vector2& v);
	Vector4& operator-=(const Vector2& v);
	Vector4& operator*=(const Vector2& v);
	Vector4& operator/=(const Vector2& v);
	Vector4& operator+=(const Vector3& v);
	Vector4& operator-=(const Vector3& v);
	Vector4& operator*=(const Vector3& v);
	Vector4& operator/=(const Vector3& v);

	const Vector4 operator+(const Vector4& v) const;
	const Vector4 operator-(const Vector4& v) const;
	const Vector4 operator*(const Vector4& v) const;
	const Vector4 operator/(const Vector4& v) const;
	const Vector4 operator+(const Vector2& v) const;
	const Vector4 operator-(const Vector2& v) const;
	const Vector4 operator*(const Vector2& v) const;
	const Vector4 operator/(const Vector2& v) const;
	const Vector4 operator+(const Vector3& v) const;
	const Vector4 operator-(const Vector3& v) const;
	const Vector4 operator*(const Vector3& v) const;
	const Vector4 operator/(const Vector3& v) const;

	Vector4& operator*=(float n);
	Vector4& operator/=(float n);
	friend Vector4 operator*(const Vector4& v, float n);
	friend Vector4 operator*(float n, const Vector4& v);
	friend Vector4 operator/(const Vector4& v, float n);
	friend Vector4 operator/(float n, const Vector4& v);

	void Normalize();
	void Project(const Vector4& other);
	void ProjectOnNormal(const Vector4& normal);

	Vector4 Lerp(const Vector4& dest, float t) const;
	Vector4 Slerp(const Vector4& dest, float t) const;
	Vector4 Nlerp(const Vector4& dest, float t) const;
	Vector4 MoveTowards(const Vector4& other, float deltaDistance) const;

	float Dot(const Vector4& v) const;
	float Angle(const Vector4& v) const;
	float GetLengthSquared() const;
	float GetLength() const;
	float DistanceSquared(const Vector4& v) const;
	float Distance(const Vector4& v) const;

	Vector4 Abs() const;
	Vector4 Normalized() const;
	Vector4 Projected(const Vector4& other) const;
	Vector4 ProjectedOnNormal(const Vector4& normal) const;

	Vector4 Floor() const;
	Vector4 Ceil() const;
	Vector4 Round() const;

	Vector4 Min(const Vector4& other) const;
	Vector4 Max(const Vector4& other) const;
	Vector4 Clamp(const Vector4& min, const Vector4& max) const;

};