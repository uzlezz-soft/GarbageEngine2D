#include "Rendering/VertexBuffer.h"
#include <glad/glad.h>

VertexBuffer::VertexBuffer(const void* data, uint32 size, uint32 count, bool dynamic, bool instanced) : m_dynamic(dynamic), m_instanced(instanced), m_count(count)
{
	glGenBuffers(1, &m_id);
	Bind();
	m_size = size;
	m_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	glBufferData(GL_ARRAY_BUFFER, size, data, m_usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::UpdateData(const void* data, uint32 size)
{
	// It is not required to clear the buffer before writing new data to it
	// https://stackoverflow.com/questions/24512468/when-to-clear-a-vertex-buffer-object
	// ...
	// However, i have done some tests and revealed, that when i clear the buffer, the memory stops growing a fucking ton
	// So I'll clear the buffer anyway...
 
	glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, m_usage);
	m_size = size;
	glBufferData(GL_ARRAY_BUFFER, size, data, m_usage);
}
