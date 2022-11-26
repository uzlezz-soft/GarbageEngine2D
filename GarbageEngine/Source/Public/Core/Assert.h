#pragma once

#include "Core/Base.h"
#include "Core/Log.h"
#include <filesystem>

#ifdef GARBAGE_ENABLE_ASSERTS

#define GARBAGE_EXPAND_MACRO(x) x
#define GARBAGE_STRINGIFY_MACRO(x) #x

#define GET_LOG_MACRO_ERROR(x) GARBAGE##x##ERROR

#define GARBAGE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { GARBAGE_EXPAND_MACRO(GARBAGE_EXPAND_MACRO(GET_LOG_MACRO_ERROR(type))(msg, __VA_ARGS__)); GARBAGE_DEBUGBREAK(); } }
#define GARBAGE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) GARBAGE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define GARBAGE_INTERNAL_ASSERT_NO_MSG(type, check, ...) GARBAGE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", GARBAGE_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define GARBAGE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define GARBAGE_INTERNAL_ASSERT_GET_MACRO(...) GARBAGE_EXPAND_MACRO( GARBAGE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, GARBAGE_INTERNAL_ASSERT_WITH_MSG, GARBAGE_INTERNAL_ASSERT_NO_MSG) )

#define GARBAGE_ASSERT(expression, ...) GARBAGE_EXPAND_MACRO( GARBAGE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, expression, __VA_ARGS__) )
#define GARBAGE_CORE_ASSERT(expression, ...) GARBAGE_EXPAND_MACRO( GARBAGE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, expression, __VA_ARGS__) )

#define CORE_PURE_VIRTUAL(...) { GET_LOG_MACRO_ERROR(_CORE_)("{0}: The method is pure virtual and should be overriden in child classes!", GetType()->GetName()); GARBAGE_DEBUGBREAK(); __VA_ARGS__; }
#define PURE_VIRTUAL(...) { GET_LOG_MACRO_ERROR(_)("{0}: The method is pure virtual and should be overriden in child classes!", GetType()->GetName()); GARBAGE_DEBUGBREAK(); __VA_ARGS__; }

#else
#define GARBAGE_ASSERT(...)
#define GARBAGE_CORE_ASSERT(...)
#define CORE_PURE_VIRTUAL() { GARBAGE_BREAKPOINT(); }
#define PURE_VIRTUAL() { GARBAGE_BREAKPOINT(); }
#endif