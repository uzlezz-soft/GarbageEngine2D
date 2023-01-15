#pragma once

#include "Core/Minimal.h"
#include "Core/FileSystem/FileSystem.h"
#include <iosfwd>
#include <filesystem>
#include "Asset.generated.h"

/* Decorators, just for intellisense */

#define AssetType(type)
#define SourceFileFormats(...)
#define ConvertedFormat(format)

GCLASS();
class GARBAGE_API Asset : public ObjectBase
{
	GENERATED_BODY()

public:

	const std::filesystem::path& GetSourcePath() const { return m_sourcePath; }
	const std::filesystem::path& GetPath() const { return m_path; }
	const std::filesystem::path& GetName() const { return m_name; }

	bool JustLoadedFromSourceFile() const { return m_path.generic_string().find("Source/") == 0; }

private:

	friend class AssetManager;

	GPROPERTY();
	std::filesystem::path m_sourcePath;
	GPROPERTY();
	std::filesystem::path m_path;
	GPROPERTY();
	std::filesystem::path m_name;
	GPROPERTY();
	uint64 m_uuid;

};

GCLASS();
class GARBAGE_API AssetFactory : public ObjectBase
{
	GENERATED_BODY();

public:

	virtual bool CreateFromSourceAsset(Asset* output, File* stream, std::string_view sourceFileExtension) CORE_PURE_VIRTUAL(return false);
	virtual bool Serialize(Asset* asset, File* stream) CORE_PURE_VIRTUAL(return false);
	virtual bool Deserialize(Asset* asset, File* stream) CORE_PURE_VIRTUAL(return false);

};