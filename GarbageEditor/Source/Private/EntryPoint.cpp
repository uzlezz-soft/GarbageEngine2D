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

	Matrix4 view = Matrix4::LookAt(Vector3(0, 0, -1), Vector3(0, 0, 0), Vector2(0, 1));

	Matrix4 model = Matrix4::Identity;

	renderer.DisableFeature(Renderer::Feature::CullFace);

	Timer timer;

	uint64 currentTexture = 0;

	renderer.EnableFeature(Renderer::Feature::AlphaBlending);
	renderer.SetBlendMode(Renderer::BlendMode::Default);
	
	Random random;

	while (window.IsOpened())
	{
		window.PollEvents();

		const float aspect = window.GetFramebufferSize().X / window.GetFramebufferSize().Y;

		const Matrix4 projection = Matrix4::Ortho(-4.0f * aspect, 4.0f * aspect, -4.0f, 4.0f, -1.0f, 1.0f);

		renderer.BeginNewFrame(projection, Matrix4(1.0f));
		renderer.Clear();

		model = Matrix4::Identity.Translate(Vector3(Math::Sin(timer.GetElapsedSeconds()) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color::White);

		model = Matrix4::Identity.Translate(Vector3(-Math::Sin(timer.GetElapsedSeconds() * 2.5f) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color::Red);

		for (uint32 i = 0; i < 20000; i++)
		{
			model = Matrix4::Identity.Translate(random.UnitVector3() * 4.0f).Scale(Vector3(0.1f));

			renderer.DrawQuad(model, random.NextColor());
		}

		renderer.EndFrame();

		auto stats = renderer.GetStatistics();
		window.SetTitle(std::to_string(stats.DrawCalls) + " draw call(s) | " + std::to_string(stats.TotalNumberOfVertices) + " vertices");

		window.SwapBuffers();
	}

	return 0;
}