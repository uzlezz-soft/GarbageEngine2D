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

	std::unordered_set<std::filesystem::path> loadedTextures;
	std::vector<Ref<Texture2D>> textures;

	Timer texturesTimer;

	for (auto& entry : *fileSystem)
	{
		auto asset = AssetManager::LoadAsset(entry.Path);
		if (asset && asset->IsA<Texture2DAsset>())
		{
			auto name = asset->GetName().stem();
			if (loadedTextures.find(name) != loadedTextures.end()) continue;

			loadedTextures.insert(name);

			Texture2DAsset* textureAsset = (Texture2DAsset*)asset.get();

			GARBAGE_INFO("Texture: \"{}\" ({}x{}x{}, {})", asset->GetPath().generic_string(), textureAsset->GetSize().X, textureAsset->GetSize().Y, textureAsset->GetNumberOfColorChannels(),
				Utils::ConvertBytesQuantityToHumanReadableFormat(textureAsset->GetSize().X * textureAsset->GetSize().Y * textureAsset->GetNumberOfColorChannels()));

			Texture::Specification specification;
			specification.Format = Texture::FormatFromNumberOfColorChannels(textureAsset->GetNumberOfColorChannels());
			specification.Width = textureAsset->GetSize().X;
			specification.Height = textureAsset->GetSize().Y;
			specification.GenerateMipmaps = true;
			specification.Data = (void*)textureAsset->GetData();
			
			Ref<Texture2D> texture = MakeRef<Texture2D>(specification);

			AssetManager::SaveAsset(asset.get(), entry.Path.root_directory() / (entry.Name.stem().generic_string() + ".gbtex2d"));

			textures.push_back(texture);
		}
	}

	GARBAGE_INFO("Textures loaded in {}ms", texturesTimer.GetElapsedMilliseconds());

	float quadQuadData[] =
	{
		// positions   // texCoords
		-0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  1.0f, 0.0f,

		-0.5f,  0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  1.0f, 1.0f
	};

	VertexArray quadVertexArray;

	VertexBuffer quadDataBuffer(quadQuadData, sizeof(quadQuadData), 6);
	VertexBufferLayout quadLayout;
	quadLayout.Push<Vector2>(1);
	quadLayout.Push<Vector2>(1);
	quadVertexArray.AddBuffer(quadDataBuffer, quadLayout);

	std::unordered_map<Shader::Type, std::string_view> shaderSources;
	shaderSources[Shader::Type::Vertex] = R"(
			layout (location = 0) in vec2 a_Position;
			layout (location = 1) in vec2 a_TexCoord;
			
			out vec2 TexCoord;
			
			uniform mat4 u_mvp;
			
			void main()
			{
				gl_Position = u_mvp * vec4(a_Position, 0.0, 1.0);
				TexCoord = a_TexCoord;
			}
)";

	shaderSources[Shader::Type::Fragment] = R"(
		out vec4 Color;
		
		in vec2 TexCoord;
		
		uniform sampler2D u_texture;
		
		void main()
		{
			Color = texture(u_texture, TexCoord);
		}
)";

	Shader shader(shaderSources);

	Matrix4 view = Matrix4::LookAt(Vector3(0, 0, -1), Vector3(0, 0, 0), Vector2(0, 1));

	Matrix4 model(1.0f);

	renderer.DisableFeature(Renderer::Feature::CullFace);

	Texture::Specification whiteTextureSpecification;
	whiteTextureSpecification.Format = Texture::Format::RGB8;
	whiteTextureSpecification.Width = whiteTextureSpecification.Height = 1;
	
	uint32 whiteTextureData = 0xFFFFFFFF;

	whiteTextureSpecification.Data = &whiteTextureData;

	Texture2D whiteTexture(whiteTextureSpecification);

	Timer timer;
	Timer changeTextureTimer;

	uint64 currentTexture = 0;

	GARBAGE_INFO("Using {} vram for textures", Utils::ConvertBytesQuantityToHumanReadableFormat(MemoryStatistics::GetVRamUsedForTextures()));

	while (window.IsOpened())
	{
		window.PollEvents();

		const float aspect = window.GetFramebufferSize().X / window.GetFramebufferSize().Y;

		const Matrix4 projection = Matrix4::Ortho(-4.0f * aspect, 4.0f * aspect, -4.0f, 4.0f, -1.0f, 1.0f);

		renderer.BeginNewFrame(projection, view);
		renderer.Clear();

		model = Matrix4(1.0f).Translate(Vector3(Math::Sin(timer.GetElapsedSeconds()), 0.0f, 0.0f));

		shader.Bind();
		shader.SetMatrix4("u_mvp", projection * model);

		if (changeTextureTimer.GetElapsedSeconds() >= 1.0f)
		{
			if (++currentTexture >= textures.size()) currentTexture = 0;

			changeTextureTimer.Reset();
		}

		auto texture = textures[currentTexture];
		texture->Bind(0);
		shader.SetInt32("u_texture", 0);

		renderer.DrawVertexArray(quadVertexArray);

		renderer.EndFrame();

		window.SwapBuffers();
	}

	return 0;
}