#include "Math/Math.h"
#include "Math/Random.h"
#include <cmath>

static Random s_random;

float Math::SmallNumber = 0.000001f;
// There are never too many Pi digits... Muhahahha!
float Math::Pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899f;
float Math::PiHalf = Pi / 2.f;
float Math::Rad2Deg = 1.0f / Pi * 180.f;
float Math::Deg2Rad = 1.0f / 180.f * Pi;

float Math::Sin(float x) { return sinf(x); }
float Math::Cos(float x) { return cosf(x); }
float Math::Tan(float x) { return tanf(x); }
float Math::Ctan(float x) { return 1 / tanf(x); }

float Math::Asin(float x) { return asinf(x); }
float Math::Acos(float x) { return acosf(x); }
float Math::Atan(float x) { return atanf(x); }
float Math::Atan2(float y, float x) { return atan2f(y, x); }
float Math::Actan(float x) { return PiHalf - Atan(x); }

float Math::Exp(float x) { return expf(x); }
float Math::Log(float x) { return logf(x); }
float Math::Log10(float x) { return log10f(x); }

float Math::Pow(float x, float exp) { return powf(x, exp); }
float Math::Sqrt(float x) { return sqrtf(x); }
float Math::Cbrt(float x) { return cbrtf(x); }
float Math::Hypotenuse(float x, float y) { return hypotf(x, y); }

float Math::Min(float x, float y) { return fminf(x, y); }
float Math::Max(float x, float y) { return fmaxf(x, y); }

float Math::Ceil(float x) { return ceilf(x); }
float Math::Floor(float x) { return floorf(x); }
float Math::FMod(float x, float y) { return fmodf(x, y); }
float Math::Trunc(float x) { return truncf(x); }
int   Math::Round(float x) { return static_cast<int>(roundf(x)); }
float Math::CopySign(float number, float sign) { return copysignf(number, sign); }

float Math::FDim(float x, float y) { return fdimf(x, y); }
float Math::Abs(float x) { return abs(x); }
float Math::MultiplyAdd(float x, float y, float z)
{
#ifdef FP_FAST_FMA
    return fma(x, y, z);
#else
    return x * y + z;
#endif
}

float Math::Clamp(float value, float min, float max) { return Min(min, Max(value, max)); }
float Math::Clamp01(float value) { return Clamp(value, 0, 1); }
float Math::Lerp(float a, float b, float t) { return a + Clamp01(t) * (b - a); }
float Math::LerpUnclamped(float a, float b, float t) { return a + t * (b - a); }

bool Math::IsNaN(float a) { return std::isnan(a); }
bool Math::IsFinite(float a) { return std::isfinite(a); }
bool Math::IsInfinite(float a) { return std::isinf(a); }

float Math::CircleLength(float radius) { return 2.0f * Pi * radius; }
float Math::CircleArea(float radius) { return Pi * Square(radius); }
float Math::SphereVolume(float radius) { return Pi * 4.0f / 3.0f * radius * radius * radius; }

float Math::CyllinderBaseArea(float radius) { return CircleArea(radius); }
float Math::CyllinderSideSurfaceArea(float radius, float height) { return CircleLength(radius) * height; }
float Math::CyllinderArea(float radius, float height) { return 2.0f * CyllinderBaseArea(radius) + CyllinderSideSurfaceArea(radius, height); }
float Math::CyllinderVolume(float radius, float height) { return Pi * Square(radius) * height; }

float Math::RandomFloat() { return s_random.NextFloat(); }
float Math::RandomFloat(float min, float max) { return s_random.NextFloat(min, max); }

int32 Math::RandomInt32(int32 max) { return s_random.Next(max); }
int32 Math::RandomInt32(int32 min, int32 max) { return s_random.Next(min, max); }