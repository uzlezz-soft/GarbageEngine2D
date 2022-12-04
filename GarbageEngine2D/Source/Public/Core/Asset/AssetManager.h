#pragma once

#include "Core/Minimal.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Asset/Asset.h"

class GARBAGE_API AssetManager final : public ObjectBase
{
public:

	static void Init(Ref<FileSystem> fileSystem);

	static Ref<Asset> LoadAsset(const std::filesystem::path& name);

private:

	Ref<FileSystem> m_fileSystem{ nullptr };

	AssetManager();

	static AssetManager& Get()
	{
		static AssetManager instance;
		return instance;
	}

};