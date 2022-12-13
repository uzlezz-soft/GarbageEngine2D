#pragma once

#include "Core/Base.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include <random>

class GARBAGE_API Random
{
public:

	Random();
	Random(uint32 seed);

	int32 Next() const;
	int32 Next(int32 maxValue) const;
	int32 Next(int32 minValue, int32 maxValue) const;

	float NextFloat() const;
	float NextFloat(float minValue, float maxValue) const;

	Vector2 NextVector2(Vector2 minValue, Vector2 maxValue) const
	{
		return Vector2(NextFloat(minValue.X, maxValue.X), NextFloat(minValue.Y, maxValue.Y));
	}

	Vector3 NextVector3(Vector3 minValue, Vector3 maxValue) const
	{
		return Vector3(NextFloat(minValue.X, maxValue.X), NextFloat(minValue.Y, maxValue.Y), NextFloat(minValue.Z, maxValue.Z));
	}

	Vector4 NextVector4(Vector4 minValue, Vector4 maxValue) const
	{
		return Vector4(NextFloat(minValue.X, maxValue.X), NextFloat(minValue.Y, maxValue.Y), NextFloat(minValue.Z, maxValue.Z), NextFloat(minValue.W, maxValue.W));
	}

	Vector4 NextColor(Color minValue, Color maxValue) const
	{
		return Vector4(NextFloat(minValue.R, maxValue.R), NextFloat(minValue.G, maxValue.G), NextFloat(minValue.B, maxValue.B), NextFloat(minValue.A, maxValue.A));
	}

	Color NextColor() const
	{
		return Color(NextFloat(), NextFloat(), NextFloat(), NextFloat());
	}

	Vector3 UnitVector2() const
	{
		return Vector2(NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f)).Normalized();
	}

	Vector3 UnitVector3() const
	{
		return Vector3(NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f)).Normalized();
	}

	Vector3 UnitVector4() const
	{
		return Vector4(NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f)).Normalized();
	}

private:

	mutable std::mt19937 m_randomNumberGenerator;

};