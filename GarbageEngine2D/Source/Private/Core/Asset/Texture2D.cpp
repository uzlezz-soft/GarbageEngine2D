#include "Core/Asset/Texture2D.h"
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

bool Texture2DAssetFactory::CreateFromSourceAsset(Asset* output, File* file, std::string_view sourceFileExtension)
{
	auto rawData = file->ReadToEnd();

	int x = 0, y = 0, numColorChannels = 0;

	stbi_set_flip_vertically_on_load(true);
	uint8* data = (uint8*)stbi_load_from_memory((const stbi_uc*)rawData, file->GetEntry().Size, &x, &y, &numColorChannels, 0);

	delete[] rawData;

	if (!data) return false;

	Texture2DAsset* textureAsset = (Texture2DAsset*)output;

	textureAsset->m_data = Ref<uint8[]>(data);

	textureAsset->m_size = Vector2((float)x, (float)y);
	textureAsset->m_numberOfColorChannels = numColorChannels;

	//stbi_image_free(data);

	return true;
}