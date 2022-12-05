#include "Rendering/Texture.h"
#include "Math/Math.h"
#include "Memory/Statistics.h"
#include "OpenGL.h"
#include "Core/Assert.h"
#include <type_traits>

static uint8 GetTextureFormatSize(Texture::Format format)
{
	switch (format)
	{
		case Texture::Format::Red: return             1;
		case Texture::Format::RGB8: return            3;
		case Texture::Format::RGBA8: return           4;
		case Texture::Format::Depth16: return         2;
		case Texture::Format::Depth24Stencil8: return 4;
		case Texture::Format::Depth32: return         4;
	}

	const uint32 format_ = (uint32)format;
	GARBAGE_CORE_ASSERT(false, "Unknown texture format: {}", format_);

	return 0;
}

static uint32 GarbageEngineFormatToOpenGLFormat(Texture::Format format)
{
	switch (format)
	{
		case Texture::Format::Red: return   GL_RED;
		case Texture::Format::RGB8: return  GL_RGB;
		case Texture::Format::RGBA8: return GL_RGBA;
		case Texture::Format::Depth16: return GL_RG8;
		case Texture::Format::Depth24Stencil8: return GL_RGBA8;
		case Texture::Format::Depth32: return GL_RGBA8;
	}

	const uint32 format_ = (uint32)format;
	GARBAGE_CORE_ASSERT(false, "Unknown texture format: {}", format_);

	return 0;
}

static uint32 GarbageEngineFormatToOpenGLInternalFormat(Texture::Format format)
{
	switch (format)
	{
		case Texture::Format::Red: return   GL_R8;
		case Texture::Format::RGB8: return  GL_RGB8;
		case Texture::Format::RGBA8: return GL_RGBA8;
		case Texture::Format::Depth16: return GL_R16;
		case Texture::Format::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		case Texture::Format::Depth32: return GL_R32F;
	}

	GARBAGE_CORE_ASSERT(false, "Unknown texture format: {}", (uint32)format);

	return 0;
}

static uint32 GarbageEngineWrapModeToOpenGLWrapMode(Texture::WrapMode wrapMode)
{
	switch (wrapMode)
	{
		case Texture::WrapMode::Repeat: return         GL_REPEAT;
		case Texture::WrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case Texture::WrapMode::ClampToEdge: return    GL_CLAMP_TO_EDGE;
		case Texture::WrapMode::ClampToBorder: return  GL_CLAMP_TO_BORDER;
	}

	const uint32 wrapMode_ = (uint32)wrapMode;
	GARBAGE_CORE_ASSERT(false, "Unknown texture wrap mode: {}", wrapMode_);

	return 0;
}

static uint32 GarbageEngineFilteringToOpenGLFiltering(Texture::Filtering filtering, bool generateMipmaps = true)
{
	switch (filtering)
	{
		case Texture::Filtering::Linear:  return generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
		case Texture::Filtering::Nearest: return generateMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST;
	}

	const uint32 filtering_ = (uint32)filtering;
	GARBAGE_CORE_ASSERT(false, "Unknown texture filtering type: {}", filtering_);

	return 0;
}

template <typename T>
concept Number = std::is_arithmetic_v<T>;

template <Number T>
FORCEINLINE static bool IsPowerOfTwo(T n)
{
	return n >= 2 && (Math::Ceil(Math::Log(n)) == Math::Floor(Math::Log(n)));
}

static uint32 GetNumberOfMipLevels(uint16 resolutionX, uint16 resolutionY)
{
	uint8 mipLevels = 0;
	uint16 resX = resolutionX;
	uint16 resY = resolutionY;

	do
	{
		mipLevels++;
		resX /= 2;
		resY /= 2;
	} while (IsPowerOfTwo(resX) && IsPowerOfTwo(resY));

	return mipLevels;
}

Texture::Format Texture::FormatFromNumberOfColorChannels(uint8 numberOfColorChannels)
{
	GARBAGE_CORE_ASSERT(numberOfColorChannels > 0 && numberOfColorChannels <= 4);

	switch (numberOfColorChannels)
	{
		case 1: return Texture::Format::Red;
		case 3: return Texture::Format::RGB8;
		case 4: return Texture::Format::RGBA8;
	}

	GARBAGE_CORE_ASSERT(false, "Unknown format for this number of color channels: {}", numberOfColorChannels);

	return Texture::Format::Red;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

void Texture::Bind(uint8 slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + (uint32)slot));
	GLCall(glBindTexture(m_type, m_id));
}

bool Texture::operator==(const Texture& other)
{
	return m_type == other.m_type && m_id == other.m_id;
}

bool Texture::Setup(const Specification& specification)
{
	m_width = specification.Width;
	m_height = specification.Height;

	uint32 wrapMode = GarbageEngineWrapModeToOpenGLWrapMode(specification.WrapMode);

	bool generateMipmaps = specification.GenerateMipmaps && IsPowerOfTwo(specification.Width) && IsPowerOfTwo(specification.Height);

	uint32 minFiltering = GarbageEngineFilteringToOpenGLFiltering(specification.MinFiltering, generateMipmaps);
	uint32 magFiltering = GarbageEngineFilteringToOpenGLFiltering(specification.MagFiltering, false);

	glGenTextures(1, &m_id);
	Bind(0);

	glTexParameteri(m_type, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(m_type, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, minFiltering);
	glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, magFiltering);

	glTexParameteri(m_type, GL_TEXTURE_MAX_LEVEL, 1000);

	m_format = specification.Format;
	m_convertedFormat = GarbageEngineFormatToOpenGLFormat(specification.Format);
	m_internalFormat = GarbageEngineFormatToOpenGLInternalFormat(specification.Format);

	return generateMipmaps;
}

void Texture::UpdateMemoryInfo(uint64 size)
{
	MemoryStatistics::Get().m_vramUsedForTextures -= m_sizeInVRam;
	m_sizeInVRam = size;
	MemoryStatistics::Get().m_vramUsedForTextures += size;
}



Texture2D::Texture2D(const Specification& specification) : Texture(GL_TEXTURE_2D)
{
	bool canGenerateMipmaps = Setup(specification);

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormat(), GetWidth(), GetHeight(), 0, GetConvertedFormat(), GL_UNSIGNED_BYTE, nullptr));
	UpdateMemoryInfo((uint64)GetWidth() * (uint64)GetHeight() * (uint64)GetTextureFormatSize(specification.Format));

	if (specification.Data)
	{
		SetData(specification.Data, GetWidth() * GetHeight() * GetTextureFormatSize(GetFormat()));

		if (canGenerateMipmaps)
		{
			GLCall(glGenerateMipmap(GL_TEXTURE_2D));
		}
	}
}

void Texture2D::SetData(void* data, uint32 dataSize)
{
	const uint32 exceptedDataSize = GetWidth() * GetHeight() * GetTextureFormatSize(GetFormat());
	GARBAGE_CORE_ASSERT(dataSize == exceptedDataSize);
	GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GetWidth(), GetHeight(), GetConvertedFormat(), GL_UNSIGNED_BYTE, data));
}