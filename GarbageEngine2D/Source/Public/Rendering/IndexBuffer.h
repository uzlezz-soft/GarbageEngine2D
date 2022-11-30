#pragma once

#include "Core/Base.h"

class GARBAGE_API IndexBuffer final
{
public:

	IndexBuffer(const uint32* data, uint32 count);
	~IndexBuffer();

	void Bind() const;

	FORCEINLINE uint32 GetCount() const { return m_count; }

private:

	uint32 m_id{ 0 };
	uint32 m_count;

};
