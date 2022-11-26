#pragma once

#include "Core/Base.h"

class GARBAGE_API VertexBuffer final
{
public:

	/**
	  *   \brief Vertex buffer
	  *
	  *   \param data Pointer to data buffer that will be uploaded to GPU
	  *   \param size Size of the data
	  *   \param count Number of vertices
	  *   \param dynamic Is this buffer will be dynamically updated or not
	  *
	  **/
	VertexBuffer(const void* data, uint32 size, uint32 count, bool dynamic = false, bool instanced = false);
	~VertexBuffer();

	void Bind() const;

	void UpdateData(const void* data, uint32 size);

	FORCEINLINE bool IsDynamic() const { return m_dynamic; }
	FORCEINLINE bool IsInstanced() const { return m_instanced; }
	FORCEINLINE uint32 GetCount() const { return m_count; }

private:

	uint32 m_id{ 0 };
	uint32 m_usage{ 0 };
	uint32 m_size{ 0 };
	uint32 m_count{ 0 };
	bool m_dynamic = false;
	bool m_instanced = false;

};