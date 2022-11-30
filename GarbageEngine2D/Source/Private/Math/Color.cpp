#include "Math/Color.h"
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

Color Color::White = Color(1.f, 1.f, 1.f);
Color Color::Black = Color();
Color Color::Red = Color(1.f);
Color Color::Green = Color(0.f, 1.f);
Color Color::Blue = Color(0.f, 0.f, 1.f);
Color Color::Cyan = Color(0.f, 1.f, 1.f);
Color Color::Gray = Color(0.5f, 0.5f, 0.5f);
Color Color::Magenta = Color(1.f, 0.f, 1.f);
Color Color::Purple = Color(0.2588f, 0.1098f, 0.3215f);
Color Color::Yellow = Color(1.f, 0.92f, 0.016f);
Color Color::Transparent = Color(0.f, 0.f, 0.f, 0.f);

Color::Color(Vector2 vector, float b, float a) : R(vector.X), G(vector.Y), B(b), A(a) {}

Color::Color(Vector3 vector, float a) : R(vector.X), G(vector.Y), B(vector.Z), A(a) {}

Color::Color(Vector4 vector) : R(vector.X), G(vector.Y), B(vector.Z), A(vector.W) {}

Vector2 Color::GetAsVector2() const
{
	return Vector2(R, G);
}

Vector3 Color::GetAsVector3() const
{
	return Vector3(R, G, B);
}

Vector4 Color::GetAsVector4() const
{
	return Vector4(R, G, B, A);
}

Color Color::Lerp(const Color& other, float time) const
{
	return Color(Math::Lerp(R, other.R, time), Math::Lerp(G, other.G, time), Math::Lerp(B, other.B, time), Math::Lerp(A, other.A, time));
}

Color Color::Lerp(const Color& left, const Color& right, float time)
{
	return Color(Math::Lerp(left.R, right.R, time), Math::Lerp(left.G, right.G, time), Math::Lerp(left.B, right.B, time), Math::Lerp(left.A, right.A, time));
}

float Color::ConvertSrgbGammaToLinear(float x) const
{
	return x <= 0.04045f ? (x / 12.92f) : Math::Pow((x + 0.055f) / 1.055f, 2.4f);
}

float Color::ConvertSrgbLinearToGamma(float x) const
{
	return x <= 0.0031308f ? 12.92f * x : 1.055f * Math::Pow(x, 1.0f / 2.4f) - 0.055f;
}

Color Color::LinearToGamma() const
{
	return Color(ConvertSrgbLinearToGamma(R),
		ConvertSrgbLinearToGamma(G),
		ConvertSrgbLinearToGamma(B), A);
}

Color Color::GammaToLinear() const
{

	return Color(ConvertSrgbGammaToLinear(R),
		ConvertSrgbGammaToLinear(G),
		ConvertSrgbGammaToLinear(B), A);
}

const Color Color::operator+(const Color& other) const
{
	return Color(R + other.R, G + other.G, B + other.B, A + other.A);
}

void Color::operator+=(const Color& other)
{
	R += other.R;
	G += other.G;
	B += other.B;
	A += other.A;
}

const Color Color::operator-(const Color& other) const
{
	return Color(R - other.R, G - other.G, B - other.B, A - other.A);
}

void Color::operator-=(const Color& other)
{
	R -= other.R;
	G -= other.G;
	B -= other.B;
	A -= other.A;
}

const Color Color::operator*(const Color& other) const
{
	return Color(R * other.R, G * other.G, B * other.B, A * other.A);
}

const Color Color::operator*(float other) const
{
	return Color(R * other, G * other, B * other, A * other);
}

void Color::operator*=(const Color& other)
{
	R *= other.R;
	G *= other.G;
	B *= other.B;
	A *= other.A;
}

void Color::operator*=(float other)
{
	R *= other;
	G *= other;
	B *= other;
	A *= other;
}

const Color Color::operator/(const Color& other) const
{
	return Color(R / other.R, G / other.G, B / other.B, A / other.A);
}

const Color Color::operator/(float other) const
{
	return Color(R / other, G / other, B / other, A / other);
}

void Color::operator/=(const Color& other)
{
	R /= other.R;
	G /= other.G;
	B /= other.B;
	A /= other.A;
}

void Color::operator/=(float other)
{
	R /= other;
	G /= other;
	B /= other;
	A /= other;
}

Color::operator Vector2() const
{
	return Vector2(R, G);
}

Color::operator Vector3() const
{
	return Vector3(R, G, B);
}

Color::operator Vector4() const
{
	return Vector4(R, G, B, A);
}
