#pragma once

#include "Core/Base.h"
#include "Math/Math.h"
#include "Math/Vector3.h"

class GARBAGE_API GARBAGE_ALIGN(16) Quaternion
{
public:

	static Quaternion Identity;

	union
	{
		struct { float X, Y, Z, W; };
		float Data[4];
	};

	Quaternion() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0) {}
	explicit Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
	explicit Quaternion(const Vector3& vector, float w) : X(vector.X), Y(vector.Y), Z(vector.Z), W(w) {}
	Quaternion(float xyzw) : X(xyzw), Y(xyzw), Z(xyzw), W(xyzw) {}
	explicit Quaternion(const Vector3& eulerAngles);

	float GetLengthSquared() const { return X * X + Y * Y + Z * Z + W * W; }
	float GetLength() const { return Math::Sqrt(GetLengthSquared()); }
	Quaternion Normalized() const;

	float GetAngle() const noexcept { return 2.f * Math::Acos(W); }

	Vector3 GetEulerAngles() const;
	Vector3 RotateVector(const Vector3& v) const;

	Vector3 GetXAxis() const { return RotateVector(Vector3(1.f, 0.f, 0.f)); }
	Vector3 GetYAxis() const { return RotateVector(Vector3(0.f, 1.f, 0.f)); }
	Vector3 GetZAxis() const { return RotateVector(Vector3(0.f, 0.f, 1.f)); }

	Vector3 GetForwardVector() const { return GetXAxis(); }
	Vector3 GetRightVector() const { return GetYAxis(); }
	Vector3 GetUpVector() const { return GetZAxis(); }

	Quaternion Inverse() const { return Quaternion(-X, -Y, -Z, W); }

	static Quaternion LookAt(Vector3 from, Vector3 to);
	static Quaternion CreateFromTwoVectors(Vector3 unitVector0, Vector3 unitVector1);
	static Quaternion CreateFromAxisAngle(Vector3 axis, float angle);

	Quaternion operator+(const Quaternion & other) const { return Quaternion(X + other.X, Y + other.Y, Z + other.Z, W + other.W); }
	Quaternion operator-(const Quaternion & other) const { return Quaternion(X - other.X, Y - other.Y, Z - other.Z, W - other.W); }
	Quaternion operator*(float value) const { return Quaternion(X * value, Y * value, Z * value, W * value); }
	Quaternion operator/(float value) const { return Quaternion(X / value, Y / value, Z / value, W / value); }
	Quaternion operator-() const { return Inverse(); }

	void operator+=(const Quaternion & other) { *this = operator+(other); }
	void operator-=(const Quaternion & other) { *this = operator-(other); }
	void operator*=(float other) { *this = operator*(other); }
	void operator/=(float other) { *this = operator/(other); }

	bool operator==(const Quaternion & other) const { return X == other.X && Y == other.Y && Z == other.Z && W == other.W; }
	bool operator!=(const Quaternion & other) const { return !operator==(other); }

};