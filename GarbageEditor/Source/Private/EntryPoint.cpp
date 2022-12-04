#include "Editor/Core.h"
#include "Math/Vector2.h"
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include "Core/FileSystem/PhysicalFileSystem.h"
#include "Core/Utils.h"
#include "Core/Asset/AssetManager.h"
#include <Core/Asset/Texture2D.h>

int main()
{
	GarbageEditor::Init();
	Window::InitSubsystem();

	Ref<FileSystem> fileSystem = MakeRef<PhysicalFileSystem>();
	((PhysicalFileSystem*)fileSystem.get())->SetWorkingDirectory("C:/Users/User/Desktop/Assets");

	AssetManager::Init(fileSystem);

	for (auto& entry : *fileSystem)
	{
		auto asset = AssetManager::LoadAsset(entry.Path);
		if (asset && asset->IsA<Texture2DAsset>())
		{
			Texture2DAsset* textureAsset = (Texture2DAsset*)asset.get();

			GARBAGE_INFO("Texture: {} ({}x{}x{})", asset->GetName(), textureAsset->GetSize().X, textureAsset->GetSize().Y, textureAsset->GetNumberOfColorChannels());
		}
	}

	Window::Context context;
	Window window(context);

	window.Open(1280, 720, "Garbage Editor");

	Renderer renderer;
	renderer.Init();

	const Matrix4 projection = Matrix4::Ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.01f, 10.0f);
	Matrix4 view = Matrix4::LookAt(Vector3(-1, 0, 0), Vector3(0, 0, 0), Vector2(0, 1));

	while (window.IsOpened())
	{
		window.PollEvents();

		renderer.BeginNewFrame(projection, view);
		renderer.Clear();

		renderer.EndFrame();

		window.SwapBuffers();
	}

	return 0;
}