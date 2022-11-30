#include "Memory/ArenaAllocator.h"
#include "Math/Math.h"
#include <cstdlib>

ArenaAllocator::ArenaAllocator(uint64 size) : m_capacity(size)
{
	m_bytes = std::calloc(1, size);
}

ArenaAllocator::~ArenaAllocator()
{
	std::free(m_bytes);
}

void* ArenaAllocator::Allocate(uint64 size)
{
	uint64 current = Current();

	if (m_bytesWritten + size > m_capacity)
	{
		m_capacity += size;
		m_capacity = Math::Ceil((float)m_capacity * 1.5f);
		m_bytes = std::realloc(m_bytes, m_capacity);

		std::memset((void*)((uint64)m_bytes + m_bytesWritten), 0, m_capacity - m_bytesWritten);
	}

	m_bytesWritten += size;

	return (void*)current;
}

uint64 ArenaAllocator::Current() const
{
	return (uint64)m_bytes + m_bytesWritten;
}

uint64 ArenaAllocator::Length() const
{
	return m_bytesWritten;
}

void ArenaAllocator::Reserve(uint64 size)
{
	m_capacity += size;
	m_bytes = std::realloc(m_bytes, m_capacity);
}

void ArenaAllocator::Reset()
{
	m_bytes = std::calloc(1, m_capacity);
	m_bytesWritten = 0;
}
