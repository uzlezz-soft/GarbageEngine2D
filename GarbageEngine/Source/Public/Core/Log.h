#pragma once

#include "Core/Base.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Color.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

class GARBAGE_API Log
{
public:

	static void Init();

	FORCEINLINE static Ref<spdlog::logger> GetCoreLogger() { return s_coreLogger; }
	FORCEINLINE static Ref<spdlog::logger> GetClientLogger() { return s_clientLogger; }

private:

	static Ref<spdlog::logger> s_coreLogger;
	static Ref<spdlog::logger> s_clientLogger;

};

template<typename OStream>
OStream& operator<<(OStream& os, const Vector2& vec)
{
	return os << "[" << vec.X << " " << vec.Y << "]";
}

template<typename OStream>
OStream& operator<<(OStream& os, const Vector3& vec)
{
	return os << "[" << vec.X << " " << vec.Y << " " << vec.Z << "]";
}

template<typename OStream>
OStream& operator<<(OStream& os, const Vector4& vec)
{
	return os << "[" << vec.X << " " << vec.Y << " " << vec.Z << " " << vec.W << "]";
}

template<typename OStream>
OStream& operator<<(OStream& os, const Matrix4& m)
{
	return os << m.V[0][0] << " " << m.V[1][0] << " " << m.V[2][0] << " " << m.V[3][0] << "\n"
			  << m.V[0][1] << " " << m.V[1][1] << " " << m.V[2][1] << " " << m.V[3][1] << "\n"
			  << m.V[0][2] << " " << m.V[1][2] << " " << m.V[2][2] << " " << m.V[3][2] << "\n"
			  << m.V[0][3] << " " << m.V[1][3] << " " << m.V[2][3] << " " << m.V[3][3];
}

template<typename OStream>
OStream& operator<<(OStream& os, const Quaternion& quat)
{
	return os << "[" << quat.X << "rad " << quat.Y << "rad " << quat.Z << "rad " << quat.W << "rad]";
}

template<typename OStream>
OStream& operator<<(OStream& os, const Color& color)
{
	return os << "[R" << color.R << " G" << color.G << " B" << color.B << " A" << color.A << "]";
}

#define GARBAGE_CORE_DEBUG(...) ::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define GARBAGE_CORE_TRACE(...) ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define GARBAGE_CORE_INFO(...)  ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define GARBAGE_CORE_WARN(...)  ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define GARBAGE_CORE_ERROR(...) ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define GARBAGE_CORE_FATAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define GARBAGE_DEBUG(...) ::Log::GetClientLogger()->debug(__VA_ARGS__)
#define GARBAGE_TRACE(...) ::Log::GetClientLogger()->trace(__VA_ARGS__)
#define GARBAGE_INFO(...)  ::Log::GetClientLogger()->info(__VA_ARGS__)
#define GARBAGE_WARN(...)  ::Log::GetClientLogger()->warn(__VA_ARGS__)
#define GARBAGE_ERROR(...) ::Log::GetClientLogger()->error(__VA_ARGS__)
#define GARBAGE_FATAL(...) ::Log::GetClientLogger()->critical(__VA_ARGS__)