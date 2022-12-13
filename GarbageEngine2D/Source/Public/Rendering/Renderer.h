#pragma once

#include "Core/Base.h"
#include "Rendering/VertexArray.h"
#include "Rendering/IndexBuffer.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

class GARBAGE_API Renderer
{
public:

	enum class Feature
	{
		CullFace, AlphaBlending, WriteToDepthBuffer
	};

	enum class BlendMode
	{
		Default, Additive
	};

	enum class FaceCullingMode
	{
		ClockWise, CounterClockWise
	};

	enum class DepthFunction
	{
		LessOrEqual, Less, Equal, Greater, GreaterOrEqual, NotEqual, Always, Never
	};

	struct Statistics
	{
		uint32 DrawCalls{ 0 };
		uint32 TotalNumberOfVertices{ 0 };
		float FrameTime{ 0.0f };
		uint64 QuadCount{ 0 };

		void Reset()
		{
			DrawCalls = 0;
			TotalNumberOfVertices = 0;

			FrameTime = 0.0f;

			QuadCount = 0;
		}

		float GetFrameTimeSeconds() const { return FrameTime / 1000.0f; }
		float GetFrameTimeMilliseconds() const { return FrameTime; }
	};

	Renderer() = default;

	void Init();

	void Clear();

	void SetViewportSize(Vector2 viewportSize);

	void BeginNewFrame(const Matrix4& projection, const Matrix4& view);
	void EndFrame();

	void DrawVertexArray(const VertexArray& vertexArray);
	void DrawVertexArray(const VertexArray& vertexArray, const IndexBuffer& indexBuffer);
	void DrawVertexArray(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, uint64 count);

	void DrawQuad(const Matrix4& transform, const Color& color);

	void EnableFeature(Feature feature);
	void DisableFeature(Feature feature);

	void SetBlendMode(BlendMode blendMode);
	void SetDepthFunction(DepthFunction depthFunction);

	void SetFaceCullingMode(FaceCullingMode mode);

	const Statistics& GetStatistics() const;

	int32 GetNumberOfSupportedVertexAttributes() const { return m_numberOfSupportedVertexAttributes; }
	int32 GetMaxTextureSize() const { return m_maxTextureSize; }
	int32 GetNumberOfTextureUnits() const { return m_numberOfTextureUnits; }

	NON_COPYABLE(Renderer)

private:

	int32 m_numberOfSupportedVertexAttributes;
	int32 m_maxTextureSize;
	int32 m_numberOfTextureUnits;
	
	void StartBatch();
	void FlushBatch();
	void NextBatch();

};