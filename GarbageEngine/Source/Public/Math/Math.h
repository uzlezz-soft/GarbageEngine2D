#pragma once

#include "Core/Base.h"

class GARBAGE_API Math final
{
public:

	static float Pi;
	static float PiHalf;
	static float Rad2Deg;
	static float Deg2Rad;

	static float Sin(float x);
	static float Cos(float x);
	static float Tan(float x);
	static float Ctan(float x);

	static float Asin(float x);
	static float Acos(float x);
	static float Atan(float x);
	static float Atan2(float y, float x);
	static float Actan(float x);

	static float Exp(float x);
	static float Log(float x);
	static float Log10(float x);

	static float Pow(float x, float exp);
	static float Sqrt(float x);
	static float Cbrt(float x);
	static float Hypotenuse(float x, float y);

	static float Min(float x, float y);
	static float Max(float x, float y);

	static float Ceil(float x);
	static float Floor(float x);
	static float FMod(float x, float y);
	static float Trunc(float x);
	static int Round(float x);
	static float CopySign(float number, float sign);

	static float FDim(float x, float y);
	static float Abs(float x);
	static float MultiplyAdd(float x, float y, float z);

	static float Clamp(float value, float min, float max);
	static float Clamp01(float value);
	static float Lerp(float a, float b, float t);
	static float LerpUnclamped(float a, float b, float t);

	template <typename T>
	static T Square(T a) { return a * a; }

	static bool IsNaN(float a);
	static bool IsFinite(float a);
	static bool IsInfinite(float a);

	static float CircleLength(float radius);
	static float CircleArea(float radius);
	static float SphereVolume(float radius);

	static float CyllinderBaseArea(float radius);
	static float CyllinderSideSurfaceArea(float radius, float height);
	static float CyllinderArea(float radius, float height);
	static float CyllinderVolume(float radius, float height);

private:

	Math() = default;

};