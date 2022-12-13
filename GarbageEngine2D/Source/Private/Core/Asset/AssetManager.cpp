#include "Core/Asset/AssetManager.h"
#include "Core/Asset/Asset.h"
#include "Core/Registry.h"
#include <algorithm>
#include <cctype>

static bool StripFileExtension(const std::filesystem::path& name, std::string& out)
{
	auto extension_ = name.extension();

	auto extensionString = extension_.string();
	if (extensionString.size() <= 1) return false;

	out = extensionString.substr(1);

	std::transform(out.begin(), out.end(), out.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return true;
}

AssetManager::AssetManager() : m_allocator(1024)
{
	GARBAGE_CORE_INFO("Asset manager initialized");

	GatherAssetFactories();
}

void AssetManager::GatherAssetFactories()
{
	static auto parentType = Meta::Registry::Get().FindType("AssetFactory");

	m_factories.clear();
	m_allocator.Reset();
	GARBAGE_CORE_INFO("Registering asset factories...");

	for (auto& type : Meta::Registry::Get().GetAllTypes())
	{
		if (type->HasParent(parentType))
		{
			GARBAGE_CORE_ASSERT(type->HasDecorator("AssetType"));

			m_factories.emplace_back((AssetFactory*)type->Construct(&m_allocator));
			GARBAGE_CORE_TRACE("Found asset factory: {}", type->GetName());
		}
	}

	GARBAGE_CORE_INFO("Registered {} asset factories.", m_factories.size());
}

void AssetManager::Init(Ref<FileSystem> fileSystem)
{
	Get().m_fileSystem = fileSystem;
}

Ref<Asset> AssetManager::LoadAsset(const std::filesystem::path& name)
{
	std::string extension;
	if (!StripFileExtension(name, extension)) return nullptr;

	auto file = Get().m_fileSystem->OpenFile(Get().m_fileSystem->FindFile(name).value());

	std::string sourcePath;
	{
		const std::string_view assetTestString = "GARBAGE_ASSET_";

		std::string str;
		str.resize(assetTestString.size());

		file->ReadRawString((uint8*)&str[0], assetTestString.size());
		if (str == assetTestString)
		{
			*file >> sourcePath;
		}
		else
		{
			file->SetStreamPosition(0);
		}
	}

	for (auto& factory : Get().m_factories)
	{
		auto type = factory->GetType();

		if (type->HasDecorator("ConvertedFormat"))
		{
			auto convertedFileExtensions = type->GetDecoratorValues("ConvertedFormat");

			for (auto& format : *convertedFileExtensions)
			{
				if (format == extension)
				{
					auto instance = (AssetFactory*)type->Construct(nullptr);

					Ref<Asset> asset = Ref<Asset>((Asset*)Meta::Registry::Get().FindType((*type->GetDecoratorValues("AssetType"))[0])->Construct(nullptr));

					if (instance->Deserialize(asset.get(), file.get()))
					{
						asset->m_name = name.filename();
						asset->m_sourcePath = sourcePath;
						asset->m_path = name;

						return asset;
					}
				}
			}
		}

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
						asset->m_path = std::filesystem::path("Source") / asset->m_name;

						return asset;
					}
				}
			}
		}
	}

	return nullptr;
}

void AssetManager::SaveAsset(Asset* asset, const std::filesystem::path& path)
{
	FileEntry fileEntry;

	if (!Get().m_fileSystem->IsFileExists(path)) fileEntry = Get().m_fileSystem->CreateFile(path);
	else fileEntry = Get().m_fileSystem->FindFile(path).value();

	auto file = Get().m_fileSystem->OpenFile(fileEntry);

	const std::string_view assetTestString = "GARBAGE_ASSET_";
	file->WriteRawString((uint8*)assetTestString.data(), assetTestString.size());

	*file << asset->GetSourcePath().generic_string();

	auto assetType = asset->GetType();
	
	for (auto& factory : Get().m_factories)
	{
		auto type = factory->GetType();

		if ((*type->GetDecoratorValues("AssetType"))[0] == assetType->GetName())
		{
			if (factory->Serialize(asset, file.get()))
			{
				return;
			}
		}
	}
}

AssetManager::AssetType AssetManager::GetAssetType(const std::filesystem::path& name)
{
	std::string extension;
	if (!StripFileExtension(name, extension)) return AssetType::Unknown;

	for (auto& factory : Get().m_factories)
	{
		auto type = factory->GetType();

		if (type->HasDecorator("ConvertedFormat"))
		{
			auto convertedFileExtensions = type->GetDecoratorValues("ConvertedFormat");

			for (auto& format : *convertedFileExtensions)
			{
				if (format == extension) return AssetType::Cooked;
			}
		}

		if (type->HasDecorator("SourceFileFormats"))
		{
			auto sourceFileExtensions = type->GetDecoratorValues("SourceFileFormats");

			for (auto& format : *sourceFileExtensions)
			{
				if (format == extension) return AssetType::Source;
			}
		}
	}
	
	return AssetType::Unknown;
}

bool AssetManager::SourceAssetHasCookedAsset(const std::filesystem::path& name)
{
	std::string extension;
	if (!StripFileExtension(name, extension)) return false;

	auto path = name.parent_path();
	auto stem = name.stem().generic_string() + ".";

	for (auto& factory : Get().m_factories)
	{
		auto type = factory->GetType();

		if (type->HasDecorator("ConvertedFormat") && type->HasDecorator("SourceFileFormats"))
		{
			auto sourceFileExtensions = type->GetDecoratorValues("SourceFileFormats");

			for (auto& sourceFormat : *sourceFileExtensions)
			{
				if (sourceFormat == extension)
				{
					for (auto& convertedFormat : *type->GetDecoratorValues("ConvertedFormat"))
					{
						auto filename = stem + convertedFormat;

						if (Get().m_fileSystem->IsFileExists(path / filename)) return true;
					}
				}
			}
		}
	}

	return false;
}

FileSystem* AssetManager::GetFileSystem() { return Get().m_fileSystem.get(); }

void AssetManager::ReloadAssetFactories()
{
	Get().GatherAssetFactories();
}
