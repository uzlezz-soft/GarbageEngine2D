#pragma once

#include "Core/Base.h"
#include "Core/AnimationCurve.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexArray.h"
#include "Rendering/Shader.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

class ParticleSystemProxy;

class GARBAGE_API ParticleSystem
{
public:

	Vector3 WorldPosition{ 0.0f };

	enum class SimulationSpace
	{
		Local, World
	};

	struct Specification
	{
		Vector3 StartVelocityMin = Vector3(-2.0f, 5.0f, -2.0f);
		Vector3 StartVelocityMax = Vector3(2.0f, 10.0f, 2.0f);
		Vector3 StartPositionMin = Vector3(0.0f);
		Vector3 StartPositionMax = Vector3(0.0f);

		ColorAnimationCurve ColorOverLifeTime{ Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 0.0f) };
		AnimationCurve SizeOverLifeTime{ 0.0f, 1.0f };

		float LifeTimeMin = 0.75f;
		float LifeTimeMax = 1.5f;
		float GravityMultiplier = 1.0f;
		float Drag = 0.01f;

		SimulationSpace SimulationSpace = SimulationSpace::Local;
		uint64 StartCapacity = 200;
	};

	ParticleSystem(const Specification& specification, Vector3 worldPosition = Vector3(0.0f));
	ParticleSystem(const ParticleSystem& other) : ParticleSystem(other.m_specification) {}

	void Tick(float deltaTime);

	void SetProxy(ParticleSystemProxy* proxy);
	ParticleSystemProxy* GetProxy();

private:
	
	friend class ParticleSystemProxy;

	Specification m_specification;
	ParticleSystemProxy* m_proxy{ nullptr };

	struct Particle
	{
		Vector3 Velocity;
		Vector3 Position;
		Color Color;
		float LifeTime;
		float StartLifeTime;
		float Size;
		bool IsAlive{ true };
	};

	std::vector<Particle> m_particles;

};

class GARBAGE_API ParticleSystemProxy
{
public:

	ParticleSystemProxy(const ParticleSystem& particleSystem);
	ParticleSystemProxy(const ParticleSystemProxy& other) : ParticleSystemProxy(other.m_particleSystem) { /* Do nothing */ }

	void CopyData(const ParticleSystem& particleSystem);
	void CopyData();
	void Draw(const Shader& shader);

private:

	ParticleSystem& m_particleSystem;

	Scope<VertexBuffer> m_dataBuffer{ nullptr };
	Scope<VertexBuffer> m_positionsBuffer{ nullptr };
	Scope<VertexBuffer> m_colorsBuffer{ nullptr };
	Scope<VertexArray> m_vertexArray{ new VertexArray() };

	std::vector<Vector4> m_positionsSizes;
	std::vector<Color> m_colors;

	bool m_dataWasUpdated{ false };

	Vector3 m_worldPosition{ 0.0f };

	std::mutex m_copyDataMutex;

};
