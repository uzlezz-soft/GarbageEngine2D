#include "Core/Asset/Texture2D.h"
#include <stb_image/stb_image.h>

bool Texture2DAssetFactory::CreateFromSourceAsset(Asset* output, File* file, std::string_view sourceFileExtension)
{
	std::string rawData = "";
	file->ReadToEnd(rawData);

	int x = 0, y = 0, numColorChannels = 0;

	stbi_uc* data = stbi_load_from_memory((const stbi_uc*)rawData.data(), rawData.size(), &x, &y, &numColorChannels, 0);

	if (!data) return false;

	Texture2DAsset* textureAsset = (Texture2DAsset*)output;

	textureAsset->m_data = (char*)data;

	textureAsset->m_size = Vector2((float)x, (float)y);
	textureAsset->m_numberOfColorChannels = numColorChannels;

	stbi_image_free(data);

	return true;
}