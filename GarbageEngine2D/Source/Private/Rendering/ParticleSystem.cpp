#include "Rendering/ParticleSystem.h"
#include "Core/Application.h"
#include "Math/Random.h"
#include <glad/glad.h>

static Random s_random;

ParticleSystem::ParticleSystem(const Specification& specification, Vector3 worldPosition) : m_specification(specification), WorldPosition(worldPosition)
{
	m_particles.reserve(specification.StartCapacity);

	Vector3 additivePosition = m_specification.SimulationSpace == SimulationSpace::World ? WorldPosition : Vector3(0.0f);

	for (uint32 i = 0; i < specification.StartCapacity; i++)
	{
		float startLifeTime = s_random.NextFloat(specification.LifeTimeMin, specification.LifeTimeMax);

		m_particles.push_back(
			{
				s_random.NextVector3(specification.StartVelocityMin, specification.StartVelocityMax),
				s_random.NextVector3(additivePosition + specification.StartPositionMin, additivePosition + specification.StartPositionMax),
				m_specification.ColorOverLifeTime.Evaluate(0.0f),
				startLifeTime,
				startLifeTime,
				m_specification.SizeOverLifeTime.Evaluate(0.0f),
				true
			});
	}
}

void ParticleSystem::Tick(float deltaTime)
{
	Vector3 additivePosition = m_specification.SimulationSpace == SimulationSpace::World ? WorldPosition : Vector3(0.0f);

	for (auto& particle : m_particles)
	{
		if (!particle.IsAlive) continue;

		particle.LifeTime -= deltaTime;

		if (particle.LifeTime <= 0)
		{
			float startLifeTime = s_random.NextFloat(m_specification.LifeTimeMin, m_specification.LifeTimeMax);
			particle = {
				s_random.NextVector3(m_specification.StartVelocityMin, m_specification.StartVelocityMax),
				s_random.NextVector3(additivePosition + m_specification.StartPositionMin, additivePosition + m_specification.StartPositionMax),
				m_specification.ColorOverLifeTime.Evaluate(0.0f),
				startLifeTime,
				startLifeTime,
				m_specification.SizeOverLifeTime.Evaluate(0.0f),
				true
			};
			continue;
		}

		float particleLifeTimeProgress = (particle.StartLifeTime - particle.LifeTime) / particle.StartLifeTime;

		particle.Velocity += Vector3(0, -9.81f, 0) * m_specification.GravityMultiplier * deltaTime;
		particle.Velocity *= 1.0f - m_specification.Drag;
		particle.Position += particle.Velocity * deltaTime;
		particle.Size = m_specification.SizeOverLifeTime.Evaluate(particleLifeTimeProgress);
		particle.Color = m_specification.ColorOverLifeTime.Evaluate(particleLifeTimeProgress);
	}
}

void ParticleSystem::SetProxy(ParticleSystemProxy* proxy)
{
	m_proxy = proxy;
}

ParticleSystemProxy* ParticleSystem::GetProxy()
{
	return m_proxy;
}

ParticleSystemProxy::ParticleSystemProxy(const ParticleSystem& particleSystem) : m_particleSystem(*(ParticleSystem*)&particleSystem)
{
	m_particleSystem.m_proxy = this;

	static float quadData[] =
	{
		1.0f,  1.0f,  1.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f

		// positions   // texCoords
		/*-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f*/
	};

	m_dataBuffer.reset(new VertexBuffer(quadData, sizeof(quadData), sizeof(quadData) / sizeof(float)));

	VertexBufferLayout dataLayout;
	dataLayout.Push<Vector2>(1);
	dataLayout.Push<Vector2>(1);

	m_vertexArray->AddBuffer(*m_dataBuffer, dataLayout);

	m_positionsSizes.reserve(m_particleSystem.m_specification.StartCapacity);
	m_colors.reserve(m_particleSystem.m_specification.StartCapacity);

	m_positionsSizes.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
	m_colors.push_back({ 0.0f, 0.0f, 0.0f, 1.0f });

	m_positionsBuffer.reset(new VertexBuffer(m_positionsSizes.data(), m_positionsSizes.size() * sizeof(Vector4), (uint32)m_positionsSizes.size(), true));
	m_colorsBuffer.reset(new VertexBuffer(m_colors.data(), m_colors.size() * sizeof(Vector4), (uint32)m_colors.size(), true));

	VertexBufferLayout positionsDataLayout;
	positionsDataLayout.Push<Vector4>(1);
	m_vertexArray->AddBuffer(*m_positionsBuffer, positionsDataLayout);

	VertexBufferLayout colorsDataLayout;
	colorsDataLayout.Push<Vector4>(1);
	m_vertexArray->AddBuffer(*m_colorsBuffer, colorsDataLayout);
}

void ParticleSystemProxy::CopyData(const ParticleSystem& particleSystem)
{
	std::scoped_lock<std::mutex> lockGuard(m_copyDataMutex);

	m_positionsSizes.clear();
	m_colors.clear();

	for (auto& particle : particleSystem.m_particles)
	{
		if (!particle.IsAlive) continue;

		m_positionsSizes.push_back({ particle.Position, particle.Size });
		m_colors.push_back(particle.Color);
	}

	m_worldPosition = particleSystem.m_specification.SimulationSpace == ParticleSystem::SimulationSpace::Local ? particleSystem.WorldPosition : Vector3(0.0f);

	m_dataWasUpdated = true;
}

void ParticleSystemProxy::CopyData()
{
	CopyData(m_particleSystem);
}

void ParticleSystemProxy::Draw(const Shader& shader)
{
	std::scoped_lock<std::mutex> lockGuard(m_copyDataMutex);

	if (m_dataWasUpdated)
	{
		m_positionsBuffer->Bind();
		m_positionsBuffer->UpdateData(m_positionsSizes.data(), (uint32)(m_positionsSizes.size() * sizeof(Vector4)));
		m_colorsBuffer->Bind();
		m_colorsBuffer->UpdateData(m_colors.data(), (uint32)(m_colors.size() * sizeof(Vector4)));

		m_dataWasUpdated = false;
	}

	m_vertexArray->Bind();

	shader.SetVector3("u_worldPosition", m_worldPosition);

	/*auto& renderer = Application::Get().GetRenderer();
	renderer.EnableFeature(Renderer::Feature::AlphaBlending);
	renderer.DisableFeature(Renderer::Feature::WriteToDepthBuffer);
	renderer.SetBlendMode(Renderer::BlendMode::Additive);
	renderer.SetFaceCullingMode(Renderer::FaceCullingMode::CounterClockWise);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (uint32)m_positionsSizes.size());

	renderer.DisableFeature(Renderer::Feature::AlphaBlending);
	renderer.EnableFeature(Renderer::Feature::WriteToDepthBuffer);
	renderer.SetFaceCullingMode(Renderer::FaceCullingMode::ClockWise);*/
}
