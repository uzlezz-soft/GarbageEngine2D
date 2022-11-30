#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Rendering/Shader.h"
#include <unordered_map>

class GARBAGE_API ShadersLibrary
{
public:

	static Shader* Add(std::string_view name, Shader* shader)
	{
		GARBAGE_CORE_ASSERT(!Contains(name), "Shader with name {} already exist!", name);

		Get().m_shaders[name] = shader;
	}

	static Shader* Find(std::string_view name)
	{
		auto shader = Get().m_shaders.find(name);
		GARBAGE_CORE_ASSERT(shader != Get().m_shaders.end(), "Can't find shader with name {}!", name);

		return shader->second;
	}

	static bool Contains(std::string_view name)
	{
		return Get().m_shaders.find(name) != Get().m_shaders.end();
	}

private:

	std::unordered_map<std::string_view, Shader*> m_shaders;

	static ShadersLibrary& Get()
	{
		static ShadersLibrary instance;
		return instance;
	}

	ShadersLibrary() = default;
	~ShadersLibrary()
	{
		for (auto& shader : m_shaders)
		{
			delete shader.second;
		}
	}

};