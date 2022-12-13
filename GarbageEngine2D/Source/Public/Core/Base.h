#pragma once

#include "Core/PlatformDetection.h"
#include "Core/Timer.h"
#include <memory>
#include <cstdint>
#include <filesystem>

#if !defined(GARBAGE_SHIPPING) && GARBAGE_ENGINE_DLL && defined(GARBAGE_PLATFORM_WINDOWS)
#if defined(GARBAGEENGINE_EXPORTS)
#define GARBAGE_API __declspec(dllexport)
#else
#define GARBAGE_API __declspec(dllimport)
#endif
#else
#define GARBAGE_API
#endif

#if defined(GARBAGE_PLATFORM_WINDOWS)
#define GARBAGE_DEBUGBREAK() __debugbreak()
#elif defined(GARBAGE_PLATFORM_LINUX)
#include <signal.h>
#define GARBAGE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#define GARBAGE_ALIGN(x) alignas(x)

#define GARBAGE_EXPAND_MACRO(x) x
#define GARBAGE_STRINGIFY_MACRO(x) #x
#define GARBAGE_CONCAT_MACRO(a, b) a ## b

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> MakeScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#define BIT(x) (1 << x)

#define GARBAGE_BODY_MACRO_COMBINE_INNER(A, B, C, D, E) A##B##C##D##E
#define GARBAGE_BODY_MACRO_COMBINE(A, B, C, D, E) GARBAGE_BODY_MACRO_COMBINE_INNER(A, B, C, D, E)
#define GENERATED_BODY(...) GARBAGE_BODY_MACRO_COMBINE(_, CURRENT_FILE_ID, _, __LINE__, _GENERATED_BODY);

#define GPROPERTY(...)
#define GMETHOD(...)
#define GCLASS(...)
#define GSTRUCT(...)
#define GENUM(...)

#define NON_COPYABLE(T) T(const T&) = delete; T& operator=(const T&) = delete;
#define CALL_ONLY_ONCE() do { static bool __ = false; if (__) return; __ = true; } while(0)

// Accepts a method that matches the following signature: bool (std::ostream&) 
#define Serializer(method) 
// Accepts a method that matches the following signature: bool (std::istream&)
#define Deserializer(method)
// Says to Garbage Header Tool to not generate serialization/deserialization code for this property
#define DontSerialize()

#define NO_API

#define GARBAGE_ENABLE_ASSERTS