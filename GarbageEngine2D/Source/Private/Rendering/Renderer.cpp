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

struct QuadVertex
{
	// Using Vector4 instead of Vector2 for Position is because of aligning issues :(
	Vector4 Position;
	Color Color;
	Vector2 TexCoord;
	float TexIndex{ 0.0f };
	float Tiling{ 1.0f };
};

static constexpr uint64 QuadVertexCount = 4;
static constexpr uint64 QuadIndexCount = 6;

static struct Renderer2DData
{
	static const uint32_t MaxQuads = 20000;
	static const uint32_t MaxVertices = MaxQuads * QuadVertexCount;
	static const uint32_t MaxIndices = MaxQuads * QuadIndexCount;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<IndexBuffer> QuadIndexBuffer;
	Ref<Shader> QuadShader;
	Ref<Texture2D> WhiteTexture;

	Scope<QuadVertex[]> QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	Scope<const Texture2D*[]> TextureSlots;
	uint32 TextureSlotIndex = 1;

	uint32 QuadIndexCount = 0;

	Vector2 QuadVertexPositions[QuadVertexCount];

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
	layout.Push<Vector2>(1);
	layout.Push<float>(1);
	layout.Push<float>(1);

	GARBAGE_CORE_ASSERT(layout.GetStride() == sizeof(QuadVertex));

	s_data.QuadVertexArray->AddBuffer(*s_data.QuadVertexBuffer, layout);

	s_data.QuadVertexBufferBase = MakeScope<QuadVertex[]>(s_data.MaxVertices);

	uint32* quadIndices = new uint32[s_data.MaxIndices];

	uint32 offset = 0;
	for (uint32 i = 0; i < s_data.MaxIndices; i += QuadIndexCount)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 0;
		quadIndices[i + 4] = offset + 2;
		quadIndices[i + 5] = offset + 3;

		offset += QuadVertexCount;
	}

	s_data.QuadIndexBuffer = MakeRef<IndexBuffer>(quadIndices, s_data.MaxIndices);
	delete[] quadIndices;

	Texture::Specification whiteTextureSpecification;

	whiteTextureSpecification.Width = whiteTextureSpecification.Height = 1;
	uint32_t whiteTextureData = 0xffffffff;
	whiteTextureSpecification.Format = Texture::Format::RGBA8;
	whiteTextureSpecification.Data = &whiteTextureData;

	s_data.WhiteTexture = MakeRef<Texture2D>(whiteTextureSpecification);

	Shader::Sources sources;

	sources[Shader::Type::Vertex] = R"(
		layout (location = 0) in vec4 a_Position;
		layout (location = 1) in vec4 a_Color;
		layout (location = 2) in vec2 a_TexCoord;
		layout (location = 3) in float a_TexIndex;
		layout (location = 4) in float a_Tiling;
		
		out vec4 Color;
		out vec2 TexCoord;
		out float TexIndex;
		out float Tiling;
		
		uniform mat4 u_viewProjection;
		
		void main()
		{
			gl_Position = u_viewProjection * vec4(a_Position.x, a_Position.y, 0.0, 1.0);
			Color = a_Color;
			TexCoord = a_TexCoord;
			TexIndex = a_TexIndex;
			Tiling = a_Tiling;
		}
)";

	std::stringstream fragment;

	fragment << R"(
		out vec4 OutColor;
		
		in vec4 Color;
		in vec2 TexCoord;
		in float TexIndex;
		in float Tiling;
		
		uniform sampler2D u_textures[)";
		fragment.operator<<(m_numberOfTextureUnits);
		fragment << R"(];
		
		void main()
		{
			vec4 color = Color;
			
			switch (int(TexIndex))
			{
)";

		for (uint32 i = 0; i < m_numberOfTextureUnits; i++)
		{
			fragment << "case " << i << ": color *= texture2D(u_textures[" << i << "], TexCoord * Tiling); break;\n";
		}
		fragment << R"(}

			OutColor = color;
		}
)";

	std::string source = fragment.str();

	sources[Shader::Type::Fragment] = source;

	s_data.QuadShader = MakeRef<Shader>(sources);

	s_data.QuadShader->Bind();

	for (uint32 i = 0; i < m_numberOfTextureUnits; i++)
	{
		std::stringstream ss;
		ss << "u_textures[" << i << "]";
		std::string uniform = ss.str();

		s_data.QuadShader->SetInt32(uniform, i);
	}
	
	s_data.QuadVertexPositions[0] = Vector2(-0.5f, 0.5f);
	s_data.QuadVertexPositions[1] = Vector2(-0.5f, -0.5f);
	s_data.QuadVertexPositions[2] = Vector2(0.5f, -0.5f);
	s_data.QuadVertexPositions[3] = Vector2(0.5f, 0.5f);

	s_data.TextureSlots = MakeScope<const Texture2D*[]>(m_numberOfTextureUnits);
	s_data.TextureSlots[0] = s_data.WhiteTexture.get();

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

	StartBatch();
}

void Renderer::EndFrame()
{
	FlushBatch();

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
}

void Renderer::DrawQuad(const Matrix4& transform, const Color& color, const Texture2D* texture, float tiling)
{
	static const Vector2 textureCoords[] = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };

	if (s_data.QuadIndexCount + QuadIndexCount >= Renderer2DData::MaxIndices) NextBatch();

	uint32 textureIndex = 0;

	if (texture)
	{
		if (s_data.TextureSlotIndex >= m_numberOfTextureUnits)
		{
			NextBatch();

			textureIndex = 1;
			s_data.TextureSlots[s_data.TextureSlotIndex++] = texture;
		}
		else
		{
			const uint32 numberOfTextures = Math::Min(s_data.TextureSlotIndex, m_numberOfTextureUnits);

			for (uint32 i = numberOfTextures; i > 0; i--)
			{
				if (s_data.TextureSlots[i])
				{
					if (*s_data.TextureSlots[i] == *texture)
					{
						textureIndex = i;
						break;
					}
				}
				else
				{
					textureIndex = i;
					s_data.TextureSlotIndex = i;
					s_data.TextureSlots[s_data.TextureSlotIndex++] = texture;

					break;
				}
			}

			if (textureIndex == 0)
			{
				NextBatch();

				textureIndex = s_data.TextureSlotIndex;
				s_data.TextureSlots[s_data.TextureSlotIndex++] = texture;
			}
		}
	}

	for (uint64 i = 0; i < QuadVertexCount; i++)
	{
		s_data.QuadVertexBufferPtr->Position = transform * Vector4(s_data.QuadVertexPositions[i], 1.0f, 1.0f);
		s_data.QuadVertexBufferPtr->Color = color;
		s_data.QuadVertexBufferPtr->TexIndex = (float)textureIndex;
		s_data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_data.QuadVertexBufferPtr->Tiling = tiling;
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

void Renderer::StartBatch()
{
	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase.get();

	for (uint32 i = 1; i < m_numberOfTextureUnits; i++)
	{
		s_data.TextureSlots[i] = nullptr;
	}

	s_data.TextureSlotIndex = 1;
}

void Renderer::FlushBatch()
{
	if (s_data.QuadIndexCount)
	{
		uint32 dataSize = (uint32)((uint8*)s_data.QuadVertexBufferPtr - (uint8*)s_data.QuadVertexBufferBase.get());
		s_data.QuadVertexBuffer->UpdateData(s_data.QuadVertexBufferBase.get(), dataSize);

		for (uint32 i = 0; i < s_data.TextureSlotIndex; i++)
		{
			if (s_data.TextureSlots[i]) s_data.TextureSlots[i]->Bind((uint8)i);
		}

		s_data.QuadShader->Bind();
		s_data.QuadShader->SetMatrix4(s_data.QuadShader->GetUniformLocation(Shader::CachedUniform::ViewProjection), s_data.ViewProjection);
		DrawVertexArray(*s_data.QuadVertexArray, *s_data.QuadIndexBuffer, dataSize);
		s_statistics.TotalNumberOfVertices += dataSize / sizeof(QuadVertex);
	}
}

void Renderer::NextBatch()
{
	FlushBatch();
	StartBatch();
}
