#pragma once

#include "Core/Base.h"

class Vector2;
class Vector3;
class Vector4;

class GARBAGE_API GARBAGE_ALIGN(16) Color final
{
public:

	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;
	float A = 0.0f;

	static Color White;
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Cyan;
	static Color Gray;
	static Color Magenta;
	static Color Purple;
	static Color Yellow;
	static Color Transparent;

	Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f) : R(r), G(g), B(b), A(a) {}
	Color(Vector2 vector, float b = 0.f, float a = 1.f);
	Color(Vector3 vector, float a = 1.f);
	Color(Vector4 vector);

	Vector2 GetAsVector2() const;
	Vector3 GetAsVector3() const;
	Vector4 GetAsVector4() const;

	Color Lerp(const Color & other, float time) const;
	static Color Lerp(const Color & left, const Color & right, float time);

	float ConvertSrgbGammaToLinear(float x) const;
	float ConvertSrgbLinearToGamma(float x) const;

	Color LinearToGamma() const;
	Color GammaToLinear() const;

	const Color operator+(const Color & other) const;
	void operator+=(const Color & other);

	const Color operator-(const Color & other) const;
	void operator-=(const Color & other);

	const Color operator*(const Color & other) const;
	const Color operator*(float other) const;
	void operator*=(const Color & other);
	void operator*=(float other);

	const Color operator/(const Color & other) const;
	const Color operator/(float other) const;
	void operator/=(const Color & other);
	void operator/=(float other);

	explicit operator Vector2() const;
	explicit operator Vector3() const;
	explicit operator Vector4() const;

};