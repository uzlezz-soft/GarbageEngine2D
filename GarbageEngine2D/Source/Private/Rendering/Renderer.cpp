#include "Rendering/Renderer.h"
#include "Core/Assert.h"
#include "Core/Profiling.h"
#include "Rendering/Shader.h"
#include "OpenGL.h"
#pragma warning(push, 0)
#include <GLFW/glfw3.h>
#pragma warning(pop)
#include <thread>
#include <sstream>

static Renderer::Statistics s_statistics;
static Timer s_rendererTimer;

static Matrix4 s_projection{ 0.0f };
static Matrix4 s_view{ 0.0f };
static Matrix4 s_viewProjection{ 0.0f };

#ifndef GARBAGE_SHIPPING
namespace RendererLocal
{
	struct LineBatchData
	{
		Vector3 From{ 0.0f };
		Color StartColor{ 1.0f, 0.0f, 0.0f, 1.0f };
		Vector3 To{ 0.0f };
		Color EndColor{ 1.0f, 0.0f, 0.0f, 1.0f };
	};
}

static std::vector<RendererLocal::LineBatchData> s_lines;
static Scope<VertexBuffer> s_linesBuffer{ nullptr };
static Scope<VertexArray> s_linesVertexArray{ nullptr };

static Scope<Shader> s_linesShader{ nullptr };
static uint32 s_numberOfLines{ 0 };

static std::mutex s_debugMutex;
#endif

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

	std::stringstream ss;
	ss << std::this_thread::get_id();
	GARBAGE_CORE_INFO("Renderer initialized on thread {}", ss.str());
	GARBAGE_CORE_INFO("GPU Vendor: {}", vendor);
	GARBAGE_CORE_INFO("GPU: {}", renderer);

	bool anisotropicFilteringSupported = glfwExtensionSupported("GL_EXT_texture_filter_anisotropic");
	GARBAGE_CORE_ASSERT(anisotropicFilteringSupported == true);

	// Making OpenGL work with left-handed matrices
	glFrontFace(GL_CW);

	//s_shouldClearViewport = ConfigVariable::FindOrCreate("renderer.shouldClearColor", false);
	//s_debugLinesWidth = ConfigVariable::FindOrCreate("renderer.debugLinesWidth", 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifndef GARBAGE_SHIPPING
	std::unordered_map<Shader::Type, std::string_view> shaderSources;
	shaderSources[Shader::Type::Vertex] = R"(
			layout (location = 0) in vec3 a_VertexPosition;
			layout (location = 1) in vec4 a_Color;
			
			out vec4 LineColor;
			
			uniform mat4 u_viewProjection;
			
			void main()
			{
				gl_Position = u_viewProjection * vec4(a_VertexPosition, 1.0);
				LineColor = a_Color;
			}
)";

	shaderSources[Shader::Type::Fragment] = R"(
			layout (location = 0) out vec4 Color;
			
			in vec4 LineColor;
			
			void main()
			{
				Color = LineColor;
			}
)";

	s_linesShader.reset(new Shader(shaderSources));

	s_linesVertexArray.reset(new VertexArray());

	s_lines.reserve(50);
	s_lines.push_back({ Vector3::Zero, Color::White, Vector3::One, Color::White });

	s_linesBuffer.reset(new VertexBuffer(s_lines.data(), (uint32)(s_lines.size() * sizeof(RendererLocal::LineBatchData)), (uint32)s_lines.size() * 2, true));

	VertexBufferLayout linesDataLayout;
	linesDataLayout.Push<Vector3>(1);
	linesDataLayout.Push<Color>(1);
	s_linesVertexArray->AddBuffer(*s_linesBuffer, linesDataLayout);
#endif
}

void Renderer::Clear()
{
	uint32 clearBufferBits = GL_DEPTH_BUFFER_BIT;

	if (/*s_shouldClearViewport->Get<bool>()*/true)
	{
		clearBufferBits |= GL_COLOR_BUFFER_BIT;
	}

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

	s_projection = projection;
	s_view = view;
	s_viewProjection = s_projection * s_view;

#ifndef GARBAGE_SHIPPING
	s_lines.clear();
	s_numberOfLines = 0;
#endif
}

void Renderer::EndFrame()
{
#ifndef GARBAGE_SHIPPING
	std::scoped_lock<std::mutex> debugLock(s_debugMutex);

	if (s_numberOfLines > 0)
	{
		s_linesBuffer->Bind();
		s_linesBuffer->UpdateData(s_lines.data(), (uint32)(s_lines.size() * sizeof(RendererLocal::LineBatchData)));

		EnableFeature(Renderer::Feature::AlphaBlending);
		DisableFeature(Renderer::Feature::WriteToDepthBuffer);
		DisableFeature(Renderer::Feature::CullFace);
		SetBlendMode(Renderer::BlendMode::Additive);

		s_linesShader->Bind();
		s_linesShader->SetMatrix4(s_linesShader->GetUniformLocation(Shader::CachedUniform::ViewProjection), s_viewProjection);

		s_linesVertexArray->Bind();
		glLineWidth(/*(float)s_debugLinesWidth->Get<int32>()*/1.0f);
		glDrawArrays(GL_LINES, 0, s_numberOfLines);

		DisableFeature(Renderer::Feature::AlphaBlending);
		DisableFeature(Renderer::Feature::CullFace);
		EnableFeature(Renderer::Feature::WriteToDepthBuffer);
	}
#endif

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
	vertexArray.Bind();
	indexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr);

	s_statistics.DrawCalls++;
	s_statistics.TotalNumberOfVertices += vertexArray.GetNumberOfVertices();
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
	if (blendMode == BlendMode::Additive) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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

void Renderer::DebugDrawLine(Vector3 from, Vector3 to, Color color)
{
#ifndef GARBAGE_SHIPPING
	std::scoped_lock<std::mutex> debugLock(s_debugMutex);

	s_lines.push_back({ from, color, to, color });
	s_numberOfLines += 2;
#endif
}

void Renderer::DebugDrawCoordinateSystem(Vector3 location)
{
#ifndef GARBAGE_SHIPPING
	DebugDrawLine(location, location + Vector3(1.0f, 0.0f, 0.0f), Color(1.0, 0.0, 0.0, 1.0));
	DebugDrawLine(location, location + Vector3(0.0f, 1.0f, 0.0f), Color(0.0, 1.0, 0.0, 1.0));
	DebugDrawLine(location, location + Vector3(0.0f, 0.0f, 1.0f), Color(0.0, 0.0, 1.0, 1.0));
#endif
}
