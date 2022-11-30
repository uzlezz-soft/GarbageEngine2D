#pragma once

#include "Core/Log.h"

class CoreProfiler
{
public:

	CoreProfiler(std::string_view functionSignature) : m_functionSignature(functionSignature) {}

	~CoreProfiler()
	{
		GARBAGE_CORE_INFO("Function {} finished in {}ms", m_functionSignature, m_timer.GetElapsedMilliseconds());
	}

private:

	std::string_view m_functionSignature;
	Timer m_timer;

};

class GARBAGE_API Profiler
{
public:

	Profiler(std::string_view functionSignature) : m_functionSignature(functionSignature) {}

	~Profiler()
	{
		GARBAGE_INFO("Function {} finished in {}ms", m_functionSignature, m_timer.GetElapsedMilliseconds());
	}

private:

	std::string_view m_functionSignature;
	Timer m_timer;

};

#define GARBAGE_CORE_PROFILE_FUNCTION() CoreProfiler __GARBAGE_FUNC_PROFILER(__FUNCTION__);
#define GARBAGE_PROFILE_FUNCTION() Profiler __GARBAGE_FUNC_PROFILER(__FUNCTION__);