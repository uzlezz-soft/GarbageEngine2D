#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include <vector>

struct GARBAGE_API VertexBufferElement final
{
	uint32 Count;
	uint32 Type;
	bool Normalized;
};

struct GARBAGE_API VertexBufferLayout final
{
	
	VertexBufferLayout() = default;

	template <typename T>
	void Push(uint32 count)
	{
		GARBAGE_CORE_ASSERT(false);
	}

	template <>
	void Push<float>(uint32 count)
	{
		m_elements.push_back({ count, 0x1406, false });
		m_stride += sizeof(float) * count;
	}

	template <>
	void Push<uint32>(uint32 count)
	{
		m_elements.push_back({ count, 0x1405, false });
		m_stride += sizeof(uint32) * count;
	}

	template <>
	void Push<uint8>(uint32 count)
	{
		m_elements.push_back({ count, 0x1401, true });
		m_stride += sizeof(uint8) * count;
	}

	template <>
	void Push<Vector2>(uint32 count)
	{
		for (uint32 i = 0; i < count; i++) Push<float>(2);
	}

	template <>
	void Push<Vector3>(uint32 count)
	{
		for (uint32 i = 0; i < count; i++) Push<float>(3);
	}

	template <>
	void Push<Vector4>(uint32 count)
	{
		for (uint32 i = 0; i < count; i++) Push<float>(4);
	}

	template <>
	void Push<Color>(uint32 count)
	{
		for (uint32 i = 0; i < count; i++) Push<float>(4);
	}

	template <>
	void Push<Matrix4>(uint32 count)
	{
		for (uint32 i = 0; i < count; i++) Push<Vector4>(4);
	}

	FORCEINLINE uint32 GetStride() const { return m_stride; }
	FORCEINLINE const std::vector<VertexBufferElement>& GetElements() const { return m_elements; }

private:

	std::vector<VertexBufferElement> m_elements;
	uint32 m_stride{ 0 };

};