#pragma once

#include "Core/Minimal.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Asset/Asset.h"
#include "Memory/ArenaAllocator.h"

class GARBAGE_API AssetManager final : public ObjectBase
{
public:

	enum class AssetType
	{
		Source, Cooked, Unknown
	};

	static void Init(Ref<FileSystem> fileSystem);

	static Ref<Asset> LoadAsset(const std::filesystem::path& name);
	static void SaveAsset(Asset* asset, const std::filesystem::path& path);

	static AssetType GetAssetType(const std::filesystem::path& name);
	static bool SourceAssetHasCookedAsset(const std::filesystem::path& name);

	static FileSystem* GetFileSystem();

	static void ReloadAssetFactories();

private:

	Ref<FileSystem> m_fileSystem{ nullptr };

	std::vector<AssetFactory*> m_factories;

	ArenaAllocator m_allocator;

	AssetManager();

	void GatherAssetFactories();

	static AssetManager& Get()
	{
		static AssetManager instance;
		return instance;
	}

};