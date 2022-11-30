#include "Math/Random.h"
#include <random>

Random::Random() : m_randomNumberGenerator(std::_Random_device())
{
}

Random::Random(uint32 seed) : m_randomNumberGenerator(seed)
{
}

int32 Random::Next() const
{
    return Next(0, std::numeric_limits<int32>::max());
}

int32 Random::Next(int32 maxValue) const
{
    return Next(0, maxValue);
}

int32 Random::Next(int32 minValue, int32 maxValue) const
{
    std::uniform_int_distribution<std::int32_t> distribution(minValue, maxValue);
    return distribution(m_randomNumberGenerator);
}

float Random::NextFloat() const
{
    return NextFloat(0.0f, 1.0f);
}

float Random::NextFloat(float minValue, float maxValue) const
{
    std::uniform_real_distribution<std::float_t> distribution(minValue, maxValue);
    return distribution(m_randomNumberGenerator);
}
