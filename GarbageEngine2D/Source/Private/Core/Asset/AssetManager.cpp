#include "Core/Asset/AssetManager.h"
#include "Core/Asset/Asset.h"
#include "Core/Registry.h"

AssetManager::AssetManager()
{
	GARBAGE_CORE_INFO("Asset manager initialized");
}

void AssetManager::Init(Ref<FileSystem> fileSystem)
{
	Get().m_fileSystem = fileSystem;
}

Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& name)
{
	std::string extension;

	{
		auto extension_ = name.extension();

		auto extensionString = extension_.string();
		if (extensionString.size() <= 1) return nullptr;

		extension = extensionString.substr(1);
	}

	auto file = Get().m_fileSystem->OpenFile(Get().m_fileSystem->FindFile(name).value());

	static auto parentType = Meta::Registry::Get().FindType("AssetFactory");

	for (auto& type : Meta::Registry::Get().GetAllTypes())
	{
		if (type->HasParent(parentType))
		{
			if (type->HasDecorator("SourceFileFormats"))
			{
				auto sourceFileExtensions = type->GetDecoratorValues("SourceFileFormats");

				for (auto& format : *sourceFileExtensions)
				{
					if (format == extension)
					{
						auto instance = (AssetFactory*)type->Construct(nullptr);
						
						Ref<Asset> asset = Ref<Asset>((Asset*)Meta::Registry::Get().FindType((*type->GetDecoratorValues("AssetType"))[0])->Construct(nullptr));

						if (instance->CreateFromSourceAsset(asset.get(), file.get(), extension))
						{
							asset->m_name = name.filename();
							asset->m_sourcePath = std::filesystem::absolute(name);

							return asset;
						}
					}
				}
			}
		}
	}

	return nullptr;
}
