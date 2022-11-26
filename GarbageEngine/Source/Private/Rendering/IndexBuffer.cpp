#include "Rendering/IndexBuffer.h"
#include "Core/Assert.h"
#include <glad/glad.h>

IndexBuffer::IndexBuffer(const uint32* data, uint32 count) : m_count(count)
{
	GARBAGE_CORE_ASSERT(sizeof(uint32) == sizeof(GLuint));

	glGenBuffers(1, &m_id);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}
