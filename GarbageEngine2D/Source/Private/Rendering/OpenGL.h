#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"
#pragma warning(push, 0)
#include <glad/glad.h>
#pragma warning(pop)

#ifndef GARBAGE_SHIPPING
const char* OpenGLErrorToString(uint32 err) noexcept;

#define GLCall(x) do { while (glGetError() != 0); x; auto __error = glGetError(); if (__error != 0) GARBAGE_CORE_ERROR("OpenGLDebug: File {} Line {}: {}", std::filesystem::path(__FILE__).filename(), __LINE__, OpenGLErrorToString(__error)); } while (0)
#else
#define GLCall(x) x
#endif