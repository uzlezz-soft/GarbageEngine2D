#pragma once

#include "Core/Minimal.h"
#include "Core/Asset/Asset.h"
#include "Rendering/Texture.h"
#include "Texture2D.generated.h"

GCLASS();
class GARBAGE_API Texture2DAsset final : public Asset
{
	GENERATED_BODY()

public:

	Texture::Filtering MinFiltering;
	Texture::Filtering MagFiltering;
	Texture::WrapMode WrapMode;
	bool GenerateMipmaps;

	Vector2 GetSize() const { return m_size; }
	uint8 GetNumberOfColorChannels() const { return m_numberOfColorChannels; }
	uint8* GetData() const { return m_data.get(); }
	Texture::Format GetFormat() const { return m_format; }

private:

	friend class Texture2DAssetFactory;

	Vector2 m_size;
	uint8 m_numberOfColorChannels;
	Ref<uint8[]> m_data;
	Texture::Format m_format;

};

GCLASS(AssetType(Texture2DAsset), SourceFileFormats(png, jpg, tga, bmp, gif), ConvertedFormat(gbtex2d));
class GARBAGE_API Texture2DAssetFactory final : public AssetFactory
{
	GENERATED_BODY()

public:

	bool CreateFromSourceAsset(Asset* output, File* stream, std::string_view sourceFileExtension) override;
	bool Serialize(Asset* asset, File* stream) override;
	bool Deserialize(Asset* asset, File* stream) override;

};