#include "Rendering/VertexArray.h"
#include "Core/Assert.h"
#include <glad/glad.h>

static constexpr uint32 GetSizeOfType(uint32 type)
{
	switch (type)
	{
	case GL_FLOAT: return sizeof(GLfloat);
	case GL_UNSIGNED_INT: return sizeof(GLuint);
	case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
	}

	GARBAGE_CORE_ASSERT(false, "Get size of type failed: passed unknown type {}!", type);

	return 0;
}

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout)
{
	Bind();
	vertexBuffer.Bind();

	const auto& elements = layout.GetElements();
	uint64 offset = 0;
	for (uint32 i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(m_vertexAttributeIndex);

		glVertexAttribPointer(m_vertexAttributeIndex, element.Count, element.Type, element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)offset);
		if (vertexBuffer.IsInstanced()) glVertexAttribDivisor(m_vertexAttributeIndex, 1);

		offset += element.Count * GetSizeOfType(element.Type);
		m_vertexAttributeIndex++;
	}

	m_count += vertexBuffer.GetCount();
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_id);
}