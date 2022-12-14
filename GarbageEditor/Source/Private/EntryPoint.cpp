#include "Editor/Core.h"
#include "Math/Vector2.h"
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "Core/FileSystem/PhysicalFileSystem.h"
#include "Core/Utils.h"
#include "Core/Asset/AssetManager.h"
#include "Memory/Statistics.h"
#include <Core/Asset/Texture2D.h>
#include <Rendering/Texture.h>
#include <Rendering/Shader.h>
#include <Math/Random.h>
#include <unordered_set>
#include <portable-file-dialogs.h>

int main()
{
	GarbageEditor::Init();
	Window::InitSubsystem();

	auto dialog = pfd::select_folder("Select working directory", "");

	Ref<FileSystem> fileSystem = MakeRef<PhysicalFileSystem>();

	if (dialog.result().empty())
	{
		((PhysicalFileSystem*)fileSystem.get())->SetWorkingDirectory("C:/Users/User/Desktop/Assets");
	}
	else
	{
		((PhysicalFileSystem*)fileSystem.get())->SetWorkingDirectory(dialog.result());
	}

	AssetManager::Init(fileSystem);

	Window::Context context;
	Window window(context);

	window.Open(1280, 720, "Garbage Editor");

	Renderer renderer;
	renderer.Init();

	std::vector<Ref<Texture2D>> textures;

	for (auto& entry : *fileSystem)
	{
		auto asset = AssetManager::LoadAsset(entry.Path);
		if (asset && asset->IsA<Texture2DAsset>())
		{
			Texture2DAsset* textureAsset = (Texture2DAsset*)asset.get();

			GARBAGE_INFO("Texture: {} ({}x{}x{}, {})", asset->GetName(), textureAsset->GetSize().X, textureAsset->GetSize().Y, textureAsset->GetNumberOfColorChannels(),
				Utils::ConvertBytesQuantityToHumanReadableFormat(textureAsset->GetSize().X * textureAsset->GetSize().Y * textureAsset->GetNumberOfColorChannels()));

			Texture::Specification specification;
			specification.Format = Texture::FormatFromNumberOfColorChannels(textureAsset->GetNumberOfColorChannels());
			specification.Width = textureAsset->GetSize().X;
			specification.Height = textureAsset->GetSize().Y;
			specification.GenerateMipmaps = true;
			specification.Data = (void*)textureAsset->GetData();

			Ref<Texture2D> texture = MakeRef<Texture2D>(specification);

			textures.push_back(texture);
		}
	}

	Matrix4 cameraTransform = Matrix4::Identity;

	Matrix4 model = Matrix4::Identity;

	renderer.DisableFeature(Renderer::Feature::CullFace);

	Timer timer;

	uint64 currentTexture = 0;

	renderer.EnableFeature(Renderer::Feature::AlphaBlending);
	renderer.SetBlendMode(Renderer::BlendMode::Default);
	
	Random random;

	Texture2D* texture = textures.size() > 0 ? textures[Math::RandomInt32(textures.size())].get() : nullptr;

	while (window.IsOpened())
	{
		window.PollEvents();

		const float aspect = window.GetFramebufferSize().X / window.GetFramebufferSize().Y;

		const Matrix4 projection = Matrix4::Ortho(-4.0f * aspect, 4.0f * aspect, -4.0f, 4.0f, -1.0f, 1.0f);

		renderer.BeginNewFrame(projection, cameraTransform.Inverse());
		renderer.Clear();

		model = Matrix4::Identity.Translate(Vector3(Math::Sin(timer.GetElapsedSeconds()) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color::White, texture);

		model = Matrix4::Identity.Translate(Vector3(-Math::Sin(timer.GetElapsedSeconds() * 2.5f) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color::Red);

		renderer.EndFrame();

		auto stats = renderer.GetStatistics();
		window.SetTitle(std::to_string(stats.DrawCalls) + " draw call(s) | " + std::to_string(stats.TotalNumberOfVertices) + " vertices | " + std::to_string(stats.FrameTime) + "ms");

		window.SwapBuffers();
	}

	return 0;
}