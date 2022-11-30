#include "Core/Minimal.h"
#include "Math/Vector2.h"
#include <Rendering/Window.h>
#include <Rendering/Renderer.h>

int main()
{
	GarbageEngine::Init();
	Window::InitSubsystem();

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