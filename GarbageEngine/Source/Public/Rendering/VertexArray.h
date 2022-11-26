#pragma once

#include "Core/Base.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexBufferLayout.h"

class GARBAGE_API VertexArray final
{
public:

	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout);

	void Bind() const;
	FORCEINLINE uint32 GetNumberOfVertices() const { return m_count; }

private:

	uint32 m_id{ 0 };
	uint32 m_vertexAttributeIndex{ 0 };
	uint32 m_count{ 0 };

};