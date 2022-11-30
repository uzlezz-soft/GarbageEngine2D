#pragma once

#include "Core/Base.h"

class GARBAGE_API Allocator
{
public:

	Allocator(uint64 reserved = 1024) {}
	virtual ~Allocator() = default;

	virtual void* Allocate(uint64 size) = 0;

	virtual uint64 Current() const = 0;
	virtual uint64 Length() const = 0;

	virtual void Reserve(uint64 size) = 0;

};