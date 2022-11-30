#pragma once

#include "Core/Minimal.h"
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
	std::string_view GetName() const { return m_name; }

private:

	friend class AssetManager;

	GPROPERTY();
	std::filesystem::path m_sourcePath;
	GPROPERTY();
	std::filesystem::path m_path;
	GPROPERTY();
	std::string m_name;
	GPROPERTY();
	uint64 m_uuid;

};

GCLASS();
class GARBAGE_API AssetFactory : public ObjectBase
{
	GENERATED_BODY();

public:

	virtual bool CreateFromSourceAsset(Asset* output, std::istream& stream, std::string_view sourceFileExtension) CORE_PURE_VIRTUAL(return false);
	virtual void Serialize(Asset* asset, std::ostream& stream) CORE_PURE_VIRTUAL();
	virtual void Deserialize(Asset* asset, std::istream& stream) CORE_PURE_VIRTUAL();

};