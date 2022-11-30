#pragma once

#include "Core/Minimal.h"
#include "Texture2D.generated.h"

GCLASS();
class GARBAGE_API Texture2DAsset final : public Asset
{
	GENERATED_BODY()

public:



};

GCLASS(AssetType(Texture2DAsset), SourceFileFormats(png, jpg, tga, bmp, gif), ConvertedFormat(gbtex2d));
class GARBAGE_API Texture2DAssetFactory final : public AssetFactory
{
	GENERATED_BODY()

public:

	bool CreateFromSourceAsset(Asset* output, std::istream& stream, std::string_view sourceFileExtension) override { return false; }
	void Serialize(Asset* asset, std::ostream& stream) override {}
	void Deserialize(Asset* asset, std::istream& stream) override {}

};