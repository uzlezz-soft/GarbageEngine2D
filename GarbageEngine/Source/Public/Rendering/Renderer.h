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
		Additive
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

		void Reset()
		{
			DrawCalls = 0;
			TotalNumberOfVertices = 0;

			FrameTime = 0.0f;
		}
	};

	Renderer() = default;

	void Init();

	void Clear();

	void SetViewportSize(Vector2 viewportSize);

	void BeginNewFrame(const Matrix4& projection, const Matrix4& view);
	void EndFrame();

	void DrawVertexArray(const VertexArray& vertexArray);
	void DrawVertexArray(const VertexArray& vertexArray, const IndexBuffer& indexBuffer);

	void EnableFeature(Feature feature);
	void DisableFeature(Feature feature);

	void SetBlendMode(BlendMode blendMode);
	void SetDepthFunction(DepthFunction depthFunction);

	void SetFaceCullingMode(FaceCullingMode mode);

	const Statistics& GetStatistics() const;

	static void DebugDrawLine(Vector3 from, Vector3 to, Color color = Color(1.0f, 0.0f, 0.0f, 1.0f));
	static void DebugDrawCoordinateSystem(Vector3 location);

	NON_COPYABLE(Renderer)

};