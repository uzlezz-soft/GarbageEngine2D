#pragma once

#include "Core/Base.h"
#include "Math/Vector3.h"
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

	Vector3 NextVector3(Vector3 minValue, Vector3 maxValue) const
	{
		return Vector3(NextFloat(minValue.X, maxValue.X), NextFloat(minValue.Y, maxValue.Y), NextFloat(minValue.Z, maxValue.Z));
	}

	Vector3 UnitVector3() const
	{
		return Vector3(NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f), NextFloat(-1.0f, 1.0f)).Normalized();
	}

private:

	mutable std::mt19937 m_randomNumberGenerator;

};