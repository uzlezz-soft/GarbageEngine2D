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

int main()
{
	GarbageEditor::Init();
	Window::InitSubsystem();

	Ref<FileSystem> fileSystem = MakeRef<PhysicalFileSystem>();
	((PhysicalFileSystem*)fileSystem.get())->SetWorkingDirectory("C:/Users/User/Desktop/Assets");

	AssetManager::Init(fileSystem);

	Window::Context context;
	Window window(context);

	window.Open(1280, 720, "Garbage Editor");

	Renderer renderer;
	renderer.Init();

	Matrix4 view = Matrix4::LookAt(Vector3(0, 0, -1), Vector3(0, 0, 0), Vector2(0, 1));

	Matrix4 model(1.0f);

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

		model = Matrix4(1.0f).Translate(Vector3(Math::Sin(timer.GetElapsedSeconds()) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color(1.0f, 1.0f, 1.0f, 1.0f));

		model = Matrix4(1.0f).Translate(Vector3(-Math::Sin(timer.GetElapsedSeconds() * 2.5f) * 2.0f, 0.0f, 0.0f));

		renderer.DrawQuad(model, Color(1.0f, 0.0f, 0.0f, 1.0f));

		for (uint32 i = 0; i < 20000; i++)
		{
			model = Matrix4(1.0f).Translate(random.UnitVector3() * 4.0f).Scale(Vector3(0.1f));

			renderer.DrawQuad(model, Color(random.NextFloat(), random.NextFloat(), random.NextFloat(), 1.0f));
		}

		renderer.EndFrame();

		auto stats = renderer.GetStatistics();
		window.SetTitle(std::to_string(stats.DrawCalls) + " draw call(s) | " + std::to_string(stats.TotalNumberOfVertices) + " vertices");

		window.SwapBuffers();
	}

	return 0;
}