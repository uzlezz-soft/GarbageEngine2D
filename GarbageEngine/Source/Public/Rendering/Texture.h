#pragma once

#include "Core/Base.h"

class GARBAGE_API Texture
{
public:

	enum class Format
	{
		None = 0, Red = 1, RedInteger = 2, RGB8 = 3, RGBA8 = 4, Depth16, Depth24Stencil8, Depth32
	};

	enum class WrapMode
	{
		Repeat, MirroredRepeat, ClampToEdge, ClampToBorder
	};

	enum class Filtering
	{
		Linear, Nearest
	};

	struct Specification
	{
		uint16 Width{ 0 };
		uint16 Height{ 0 };
		Format Format{ Format::RGB8 };
		WrapMode WrapMode{ WrapMode::Repeat };
		Filtering MinFiltering{ Filtering::Linear };
		Filtering MagFiltering{ Filtering::Linear };
		void* Data{ nullptr };
		bool GenerateMipmaps = true;

		Specification() = default;

		Specification(const Specification& other)
		{
			Width = other.Width;
			Height = other.Height;
			Format = other.Format;
			WrapMode = other.WrapMode;
			MinFiltering = other.MinFiltering;
			MagFiltering = other.MagFiltering;
			GenerateMipmaps = other.GenerateMipmaps;
			Data = other.Data;
		}
	};

	Texture(const Specification& specification) {}
	virtual ~Texture();

	void Bind(uint8 slot = 0);

	virtual void SetData(void* data, uint32 size) = 0;

	uint32 GetWidth() const { return m_width; }
	uint32 GetHeight() const { return m_height; }
	Format GetFormat() const { return m_format; }

	bool operator==(const Texture& other);

protected:

	Texture(uint32 type) : m_type(type) {}

	// Returns true if mipmaps can be generated
	bool Setup(const Specification& specification);

	uint32 GetConvertedFormat() const { return m_convertedFormat; }
	uint32 GetInternalFormat() const { return m_internalFormat; }

private:

	uint32 m_id{ 0 };
	uint32 m_type{ 0 };
	uint32 m_convertedFormat{ 0 };
	uint32 m_internalFormat{ 0 };
	uint32 m_width{ 0 };
	uint32 m_height{ 0 };
	Format m_format{ Format::RGB8 };

};

class GARBAGE_API Texture2D final : public Texture
{
public:
	
	Texture2D(const Specification& specification);

	void SetData(void* data, uint32 size) override;

};