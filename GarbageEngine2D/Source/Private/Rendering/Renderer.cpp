#include "Rendering/Renderer.h"
#include "Core/Assert.h"
#include "Core/Profiling.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "OpenGL.h"
#pragma warning(push, 0)
#include <GLFW/glfw3.h>
#pragma warning(pop)
#include <thread>
#include <sstream>

static Renderer::Statistics s_statistics;
static Timer s_rendererTimer;

struct GARBAGE_ALIGN(32) QuadVertex
{
	// Using Vector4 instead of Vector2 for Position is because of aligning issues :(
	Vector4 Position;
	Color Color;
};

static struct Renderer2DData
{
	static const uint32_t MaxQuads = 20000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<IndexBuffer> QuadIndexBuffer;
	Ref<Shader> QuadShader;
	Ref<Texture2D> WhiteTexture;

	Scope<QuadVertex[]> QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	uint32 QuadIndexCount = 0;

	Vector2 QuadVertexPositions[4];

	Matrix4 Projection{ 0.0f };
	Matrix4 View{ 0.0f };
	Matrix4 ViewProjection{ 0.0f };
} s_data;

static uint32 GarbageEngineRenderingFeatureToOpenGL(Renderer::Feature feature)
{
	switch (feature)
	{
		case Renderer::Feature::CullFace: return GL_CULL_FACE;
		case Renderer::Feature::AlphaBlending: return GL_BLEND;
	}

	GARBAGE_CORE_ASSERT(false, "Unknown rendering feature: {}", (uint32)feature);
	return 0;
}

static uint32 GarbageEngineRenderingDepthFunctionToOpenGL(Renderer::DepthFunction function)
{
	switch (function)
	{
	case Renderer::DepthFunction::LessOrEqual: return GL_LEQUAL;
	case Renderer::DepthFunction::Less: return GL_LESS;
	case Renderer::DepthFunction::Equal: return GL_EQUAL;
	case Renderer::DepthFunction::Greater: return GL_GREATER;
	case Renderer::DepthFunction::GreaterOrEqual: return GL_GEQUAL;
	case Renderer::DepthFunction::NotEqual: return GL_NOTEQUAL;
	case Renderer::DepthFunction::Always: return GL_ALWAYS;
	case Renderer::DepthFunction::Never: return GL_NEVER;
	}

	GARBAGE_CORE_ASSERT(false, "Unknown depth function: {}", (uint32)function);
	
	return 0;
}

void Renderer::Init()
{
	GARBAGE_CORE_PROFILE_FUNCTION();
	CALL_ONLY_ONCE();

	auto openGLLoaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	GARBAGE_CORE_ASSERT(openGLLoaded, "Can't load OpenGL!");

	std::string_view vendor = (const char*)glGetString(GL_VENDOR);
	std::string_view renderer = (const char*)glGetString(GL_RENDERER);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &m_numberOfSupportedVertexAttributes);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_numberOfTextureUnits);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	s_data.QuadVertexArray = MakeRef<VertexArray>();

	s_data.QuadVertexBuffer = MakeRef<VertexBuffer>(nullptr, s_data.MaxVertices * sizeof(QuadVertex), s_data.MaxVertices, true);

	VertexBufferLayout layout;
	layout.Push<Vector4>(1);
	layout.Push<Color>(1);

	GARBAGE_CORE_ASSERT(layout.GetStride() == sizeof(QuadVertex));

	s_data.QuadVertexArray->AddBuffer(*s_data.QuadVertexBuffer, layout);

	s_data.QuadVertexBufferBase = MakeScope<QuadVertex[]>(s_data.MaxVertices);

	uint32* quadIndices = new uint32[s_data.MaxIndices];

	uint32 offset = 0;
	for (uint32 i = 0; i < s_data.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 0;
		quadIndices[i + 4] = offset + 2;
		quadIndices[i + 5] = offset + 3;

		offset += 4;
	}

	s_data.QuadIndexBuffer = MakeRef<IndexBuffer>(quadIndices, s_data.MaxIndices);
	delete[] quadIndices;

	Texture::Specification whiteTextureSpecification;

	whiteTextureSpecification.Width = whiteTextureSpecification.Height = 1;
	uint32_t whiteTextureData = 0xffffffff;
	whiteTextureSpecification.Data = &whiteTextureData;

	s_data.WhiteTexture = MakeRef<Texture2D>(whiteTextureSpecification);

	Shader::Sources sources;

	sources[Shader::Type::Vertex] = R"(
		layout (location = 0) in vec4 a_Position;
		layout (location = 1) in vec4 a_Color;
		
		out vec4 Color;
		
		uniform mat4 u_viewProjection;
		
		void main()
		{
			gl_Position = u_viewProjection * vec4(a_Position.x, a_Position.y, 0.0, 1.0);
			Color = a_Color;
		}
)";

	sources[Shader::Type::Fragment] = R"(
		out vec4 OutColor;
		
		in vec4 Color;
		
		void main()
		{
			OutColor = Color;
		}
)";

	s_data.QuadShader = MakeRef<Shader>(sources);

	s_data.QuadVertexPositions[0] = Vector2(-0.5f, 0.5f);
	s_data.QuadVertexPositions[1] = Vector2(-0.5f, -0.5f);
	s_data.QuadVertexPositions[2] = Vector2(0.5f, -0.5f);
	s_data.QuadVertexPositions[3] = Vector2(0.5f, 0.5f);

	std::stringstream ss;
	ss << std::this_thread::get_id();
	GARBAGE_CORE_INFO("Renderer initialized on thread {}", ss.str());
	GARBAGE_CORE_INFO("GPU Vendor: {}", vendor);
	GARBAGE_CORE_INFO("GPU: {}", renderer);
}

void Renderer::Clear()
{
	uint32 clearBufferBits = GL_COLOR_BUFFER_BIT;

	glClear(clearBufferBits);
}

void Renderer::SetViewportSize(Vector2 viewportSize)
{
	glViewport(0, 0, (int)viewportSize.X, (int)viewportSize.Y);
}

void Renderer::BeginNewFrame(const Matrix4& projection, const Matrix4& view)
{
	s_statistics.Reset();
	s_rendererTimer.Reset();

	s_data.Projection = projection;
	s_data.View = view;
	s_data.ViewProjection = projection * view;

	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase.get();
}

void Renderer::EndFrame()
{
	if (s_data.QuadIndexCount)
	{
		uint32 dataSize = (uint32)((uint8*)s_data.QuadVertexBufferPtr - (uint8*)s_data.QuadVertexBufferBase.get());
		s_data.QuadVertexBuffer->UpdateData(s_data.QuadVertexBufferBase.get(), dataSize);

		s_data.QuadShader->Bind();
		s_data.QuadShader->SetMatrix4(s_data.QuadShader->GetUniformLocation(Shader::CachedUniform::ViewProjection), s_data.ViewProjection);
		DrawVertexArray(*s_data.QuadVertexArray, *s_data.QuadIndexBuffer, s_data.QuadIndexCount);
	}

	s_statistics.FrameTime = s_rendererTimer.GetElapsedMilliseconds();
}

void Renderer::DrawVertexArray(const VertexArray& vertexArray)
{
	vertexArray.Bind();

	glDrawArrays(GL_TRIANGLES, 0, vertexArray.GetNumberOfVertices());

	s_statistics.DrawCalls++;
	s_statistics.TotalNumberOfVertices += vertexArray.GetNumberOfVertices();
}

void Renderer::DrawVertexArray(const VertexArray& vertexArray, const IndexBuffer& indexBuffer)
{
	DrawVertexArray(vertexArray, indexBuffer, indexBuffer.GetCount());
}

void Renderer::DrawVertexArray(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, uint64 size)
{
	vertexArray.Bind();
	indexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, (GLsizei)size, GL_UNSIGNED_INT, nullptr);

	s_statistics.DrawCalls++;
	s_statistics.TotalNumberOfVertices += vertexArray.GetNumberOfVertices();
}

void Renderer::DrawQuad(const Matrix4& transform, const Color& color)
{
	constexpr uint64 quadVertexCount = 4;

	for (uint64 i = 0; i < quadVertexCount; i++)
	{
		s_data.QuadVertexBufferPtr->Position = transform * Vector4(s_data.QuadVertexPositions[i], 1.0f, 1.0f);
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr++;
	}

	s_data.QuadIndexCount += 6;

	s_statistics.QuadCount++;
}

void Renderer::EnableFeature(Renderer::Feature feature)
{
	if (feature != Renderer::Feature::WriteToDepthBuffer) glEnable(GarbageEngineRenderingFeatureToOpenGL(feature));
	else glDepthMask(GL_TRUE);
}

void Renderer::DisableFeature(Renderer::Feature feature)
{
	if (feature != Renderer::Feature::WriteToDepthBuffer) glDisable(GarbageEngineRenderingFeatureToOpenGL(feature));
	else glDepthMask(GL_FALSE);
}

void Renderer::SetBlendMode(BlendMode blendMode)
{
	switch (blendMode)
	{
		case Renderer::BlendMode::Default: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case Renderer::BlendMode::Additive:glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	}
}

void Renderer::SetDepthFunction(DepthFunction depthFunction)
{
	glDepthFunc(GarbageEngineRenderingDepthFunctionToOpenGL(depthFunction));
}

void Renderer::SetFaceCullingMode(FaceCullingMode mode)
{
	glCullFace(mode == FaceCullingMode::ClockWise ? GL_CW : GL_CCW);
}

const Renderer::Statistics& Renderer::GetStatistics() const
{
	return s_statistics;
}