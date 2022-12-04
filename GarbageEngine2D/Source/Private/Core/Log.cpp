#include "Core/Log.h"
#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

GARBAGE_API Ref<spdlog::logger> Log::s_coreLogger;
GARBAGE_API Ref<spdlog::logger> Log::s_clientLogger;

template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Vector2& vec);
template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Vector3& vec);
template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Vector4& vec);
template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Matrix4& m);
template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Quaternion& quat);
template GARBAGE_API std::ostream& operator<< <std::ostream>(std::ostream& os, const Color& color);

void Log::Init()
{
	s_coreLogger = spdlog::stdout_color_mt("Garbage Core");
	s_coreLogger->set_level(spdlog::level::trace);
	s_coreLogger->set_pattern("%^[%T] %n: %v%$");

	s_clientLogger = spdlog::stdout_color_mt("Application");
	s_clientLogger->set_level(spdlog::level::trace);
	s_clientLogger->set_pattern("%^[%T] %n: %v%$");
}

Ref<spdlog::logger> Log::GetClientLogger()
{
	return s_clientLogger;
}
