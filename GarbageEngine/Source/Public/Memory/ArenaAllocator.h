#pragma once

#include "Memory/Allocator.h"

class GARBAGE_API ArenaAllocator final : public Allocator
{
public:

	ArenaAllocator(uint64 size = 1024);

	~ArenaAllocator();

	void* Allocate(uint64 size) override;

	uint64 Current() const override;

	uint64 Length() const override;

	void Reserve(uint64 size) override;

	void Reset();

private:

	void* m_bytes{ nullptr };
	uint64 m_bytesWritten{ 0 };
	uint64 m_capacity{ 0 };

};