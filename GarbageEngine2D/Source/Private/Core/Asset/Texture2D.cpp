#include "Core/Asset/Texture2D.h"
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

bool Texture2DAssetFactory::CreateFromSourceAsset(Asset* output, File* file, std::string_view sourceFileExtension)
{
	auto rawData = file->ReadToEnd();

	int x = 0, y = 0, numColorChannels = 0;

	stbi_set_flip_vertically_on_load(true);
	uint8* data = (uint8*)stbi_load_from_memory((const stbi_uc*)rawData, (int)file->GetEntry().Size, &x, &y, &numColorChannels, 0);

	delete[] rawData;

	if (!data) return false;

	Texture2DAsset* textureAsset = (Texture2DAsset*)output;

	textureAsset->m_data = Ref<uint8[]>(data);

	textureAsset->m_size = Vector2((float)x, (float)y);
	textureAsset->m_numberOfColorChannels = numColorChannels;

	textureAsset->m_format = Texture::FormatFromNumberOfColorChannels(numColorChannels);
	textureAsset->MinFiltering = textureAsset->MagFiltering = x <= 64 && y <= 64 ? Texture::Filtering::Nearest : Texture::Filtering::Linear;
	textureAsset->WrapMode = Texture::WrapMode::Repeat;
	textureAsset->GenerateMipmaps = true;

	return true;
}

bool Texture2DAssetFactory::Serialize(Asset* asset, File* stream)
{
	Texture2DAsset* textureAsset = (Texture2DAsset*)asset;

	uint16 width = (uint16)textureAsset->GetSize().X;
	uint16 height = (uint16)textureAsset->GetSize().Y;
	uint64 size = (uint64)width * (uint64)height * (uint64)textureAsset->GetNumberOfColorChannels();

	*stream << width << height << textureAsset->GetNumberOfColorChannels() << size;

	*stream << (uint8)textureAsset->GetFormat() << (uint8)textureAsset->MinFiltering << (uint8)textureAsset->MagFiltering << (uint8)textureAsset->WrapMode << textureAsset->GenerateMipmaps;

	stream->WriteRawString(textureAsset->m_data.get(), size);

	return true;
}

bool Texture2DAssetFactory::Deserialize(Asset* asset, File* stream)
{
	uint16 width = 0;
	uint16 height = 0;
	uint8 numberOfColorChannels = 0;
	uint64 size = 0;
	uint8 format = 0;
	uint8 minFiltering = 0;
	uint8 magFiltering = 0;
	uint8 wrapMode = 0;
	bool generateMipmaps = true;

	*stream >> width >> height >> numberOfColorChannels >> size >> format >> minFiltering >> magFiltering >> wrapMode >> generateMipmaps;

	Texture2DAsset* textureAsset = (Texture2DAsset*)asset;

	uint8* data = stream->ReadToEnd();

	textureAsset->m_data = Ref<uint8[]>(data);

	textureAsset->m_size = Vector2((float)width, (float)height);
	textureAsset->m_numberOfColorChannels = numberOfColorChannels;

	textureAsset->m_format = (Texture::Format)format;

	textureAsset->MinFiltering = (Texture::Filtering)minFiltering;
	textureAsset->MagFiltering = (Texture::Filtering)magFiltering;

	textureAsset->WrapMode = (Texture::WrapMode)wrapMode;
	textureAsset->GenerateMipmaps = generateMipmaps;

	return true;
}
