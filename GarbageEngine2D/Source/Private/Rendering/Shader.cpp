#include "Rendering/Shader.h"
#include "Core/Assert.h"
#include "Core/Utils.h"
#include "OpenGL.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>

namespace Utils
{

	enum class ShaderType
	{
		Vertex, Fragment
	};

	static std::string ShaderTypeToString(ShaderType type)
	{
		switch (type)
		{
		case Utils::ShaderType::Vertex: return "VERTEX";
		case Utils::ShaderType::Fragment: return "FRAGMENT";
		default:
			GARBAGE_CORE_ERROR("Unknown shader type: {}", std::to_string((int32)type).c_str());
			return "NONE";
		}
	}

	static uint32 ShaderTypeToOpenGLShaderType(ShaderType type)
	{
		switch (type)
		{
		case Utils::ShaderType::Vertex: return GL_VERTEX_SHADER;
		case Utils::ShaderType::Fragment: return GL_FRAGMENT_SHADER;
		default:
			GARBAGE_CORE_ERROR("Unknown shader type: {}", std::to_string((int32)type).c_str());
			return 0;
		}
	}

	static std::string PreprocessSource_Internal(const std::string& source, std::vector<std::string>& includedShaders)
	{
		std::stringstream ss;

		uint64 index = 0;
		do
		{
			if (index + 2 < source.length() && source[index] == '#' && source[index + 1] == 'i' && source[index + 2] == 'n')
			{
				const std::string wantedDirective = "#include";
				std::string directive = source.substr(index, wantedDirective.length());
				if (directive == wantedDirective)
				{
					index += wantedDirective.length();

					while (source[index] == ' ' || source[index] == '\t')
					{
						index++;
						if (index >= source.length())
						{
							GARBAGE_CORE_ERROR("ShaderPreprocessor: Invalid \"#include\" directive: {}", "end of file");
							return "ERROR";
						}
					}

					if (source[index++] != '"')
					{
						GARBAGE_CORE_ERROR("ShaderPreprocessor: Invalid \"#include\" directive: expected '\"' but instead got '{}'", source[index]);
						return "ERROR";
					}
					if (index >= source.length())
					{
						GARBAGE_CORE_ERROR("ShaderPreprocessor: Invalid \"#include\" directive: {}", "end of file");
						return "ERROR";
					}

					std::string toInclude;
					do
					{
						toInclude += source[index++];

						if (index >= source.length())
						{
							GARBAGE_CORE_ERROR("ShaderPreprocessor: Invalid \"#include\" directive: no closing quotes, {}", "end of file");
							return "ERROR";
						}
					} while (source[index] != '"');

					index++;

					std::string path = "assets/shaders/" + toInclude;
					if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
					{
						GARBAGE_CORE_ERROR("ShaderPreprocessor: Invalid \"#include\" directive: couldn't open file with name \"{}\"", path.c_str());
						return "ERROR";
					}

					bool shaderAlreadyIncluded = false;
					for (auto includedShader : includedShaders)
					{
						if (includedShader == path)
						{
							shaderAlreadyIncluded = true;
							break;
						}
					}

					if (!shaderAlreadyIncluded)
					{
						std::string includedSource = PreprocessSource_Internal(Utils::GetFileContents(path), includedShaders);
						ss << "\n" << includedSource << "\n";
						includedShaders.push_back(path);
					}
				}
			}
			else if (source[index++] != '\r') ss.operator<<(source[index - 1]);
		} while (index < source.length());

		return ss.str();
	}

	static std::string PreprocessSource(const std::string& source)
	{
		std::string output;

		std::vector<std::string> includedShaders;
		includedShaders.push_back("assets/shaders/common/common.glsl");
		output = PreprocessSource_Internal(source, includedShaders);

		return output;
	}

	static uint32 CompileShader(ShaderType type, const std::string& source)
	{
		std::string stringType = ShaderTypeToString(type);
		if (stringType == "NONE")
		{
			return -1;
		}
		std::string readyStringType = "_" + stringType + "_";

		static std::string commonShaderInclude;
		{
			if (commonShaderInclude.empty())
			{
				//commonShaderInclude = Utils::GetFileContents("assets/shaders/common/common.glsl");
			}
		}

		uint32 openGLType = ShaderTypeToOpenGLShaderType(type);

		std::stringstream ss;
		std::string preprocessedSource = PreprocessSource(source);
		if (preprocessedSource == "ERROR") return -1;
		ss << "#version 330 core\n#define " << readyStringType << "\n" << commonShaderInclude << "\n" << preprocessedSource;
		std::string newSource = ss.str();

		static uint32 n = 0;
		std::ofstream out("_" + std::to_string(n++) + ".txt");
		out << newSource.c_str();
		out.close();

		const char* cSource = newSource.c_str();
		int32 length = (int32)newSource.length();

		const int32 infoLogLength = 1024;
		int32 success;
		char infoLog[infoLogLength];

		uint32 shader = glCreateShader(openGLType);
		glShaderSource(shader, 1, &cSource, &length);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
			GARBAGE_CORE_ERROR("Shader of type {}: compilation failed. Log: {}", stringType.c_str(), infoLog);
			return -1;
		};

		return shader;
	}

}

static uint32 GarbageEngineShaderTypeToOpenGLShaderType(Shader::Type type)
{
	switch (type)
	{
		case Shader::Type::Vertex: return GL_VERTEX_SHADER;
		case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
		case Shader::Type::Geometry: return GL_GEOMETRY_SHADER;
	}
	
	GARBAGE_CORE_ASSERT(false, "Unknown shader type: {}", (uint32)type);

	return 0;
}

static std::string_view GarbageEngineShaderTypeToString(Shader::Type type)
{
	switch (type)
	{
		case Shader::Type::Vertex: return "Vertex";
		case Shader::Type::Fragment: return "Fragment";
		case Shader::Type::Geometry: return "Geometry";
	}

	GARBAGE_CORE_ASSERT(false, "Unknown shader type: {}", (uint32)type);

	return 0;
}

Shader::Shader(const Sources& sources, const Parameters& parameters) : m_id(0)
{
	//uint32 vertexShader = Utils::CompileShader(Utils::ShaderType::Vertex, source);
	//uint32 fragmentShader = Utils::CompileShader(Utils::ShaderType::Fragment, source);

	/*if (vertexShader == -1 || fragmentShader == -1)
	{
		GARBAGE_LOG_ERROR("Cannot create shader! {}", "Some stage has failed!");
		return;
	}*/

	m_id = glCreateProgram();
	//glAttachShader(m_id, vertexShader);
	//glAttachShader(m_id, fragmentShader);

	static std::unordered_set<Shader::Type> compiledShaders;
	static std::vector<uint32> compiledShaderIds;
	compiledShaders.clear();
	compiledShaderIds.clear();

	int32 success = 0;

	for (auto& shaderSource : sources)
	{
		Shader::Type type = shaderSource.first;
		GARBAGE_CORE_ASSERT(!compiledShaders.contains(type));

		if (type == Shader::Type::Geometry) GARBAGE_CORE_ASSERT(!compiledShaders.contains(Shader::Type::Fragment), "Shader program already has fragment shader attached! Geometry shader should go first!");

		std::stringstream ss;
		ss << "#version 330 core\n";
		for (auto& parameter : parameters)
		{
			ss << "#define " << parameter.first << " " << parameter.second << "\n";
		}
		ss << shaderSource.second;
		std::string newSource = ss.str();

		const char* cSource = newSource.c_str();
		int32 length = (int32)newSource.length();

		const int32 infoLogLength = 1024;
		char infoLog[infoLogLength];

		uint32 shader = glCreateShader(GarbageEngineShaderTypeToOpenGLShaderType(type));
		glShaderSource(shader, 1, &cSource, &length);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
			GARBAGE_CORE_ERROR("Shader of type {}: compilation failed. Log: {}", GarbageEngineShaderTypeToString(type), infoLog);
			GARBAGE_CORE_ASSERT(false);
		};

		GLCall(glAttachShader(m_id, shader));

		compiledShaders.insert(type);
		compiledShaderIds.push_back(shader);
	}

	GLCall(glLinkProgram(m_id));
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		const int32 infoLogLength = 1024;
		char infoLog[infoLogLength];
		glGetProgramInfoLog(m_id, infoLogLength, NULL, infoLog);

		for (auto& id : compiledShaderIds)
		{
			glDetachShader(m_id, id);
			glDeleteShader(id);
		}

		GARBAGE_CORE_ERROR("Cannot link shader! Log: {}", infoLog);
		GARBAGE_DEBUGBREAK();
	}

	for (auto& id : compiledShaderIds)
	{
		glDeleteShader(id);
	}

	CacheUniforms();
}

Shader::~Shader()
{
	if (m_id != 0)
	{
		glDeleteProgram(m_id);
	}
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

#define UNIFORM_LOCATION glGetUniformLocation(m_id, name.c_str())
#define SET_UNIFORM(Func) Func(UNIFORM_LOCATION, value)
#define SET_UNIFORM_KNOWN(Func) Func(uniform, value)

Shader::Uniform Shader::GetUniformLocation(std::string_view name) const
{
	return glGetUniformLocation(m_id, name.data());
}

Shader::Uniform Shader::GetUniformLocation(Shader::CachedUniform uniform) const
{
	return m_cachedUniforms[(uint8)uniform];
}

void Shader::SetBool(const std::string& name, bool value) const
{
	SET_UNIFORM(glUniform1i);
}

void Shader::SetBool(Shader::Uniform uniform, bool value) const
{
	SET_UNIFORM_KNOWN(glUniform1i);
}

void Shader::SetInt32(const std::string& name, int32 value) const
{
	SET_UNIFORM(glUniform1i);
}

void Shader::SetInt32(Shader::Uniform uniform, int32 value) const
{
	SET_UNIFORM_KNOWN(glUniform1i);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	SET_UNIFORM(glUniform1f);
}

void Shader::SetFloat(Shader::Uniform uniform, float value) const
{
	SET_UNIFORM_KNOWN(glUniform1f);
}

void Shader::SetVector2(const std::string& name, const Vector2& value) const
{
	glUniform2f(UNIFORM_LOCATION, value.X, value.Y);
}

void Shader::SetVector2(Shader::Uniform uniform, const Vector2& value) const
{
	glUniform2f(uniform, value.X, value.Y);
}

void Shader::SetVector3(const std::string& name, const Vector3& value) const
{
	glUniform3f(UNIFORM_LOCATION, value.X, value.Y, value.Z);
}

void Shader::SetVector3(Shader::Uniform uniform, const Vector3& value) const
{
	glUniform3f(uniform, value.X, value.Y, value.Z);
}

void Shader::SetVector4(const std::string& name, const Vector4& value) const
{
	glUniform4f(UNIFORM_LOCATION, value.X, value.Y, value.Z, value.W);
}

void Shader::SetVector4(Shader::Uniform uniform, const Vector4& value) const
{
	glUniform4f(uniform, value.X, value.Y, value.Z, value.W);
}

void Shader::SetColor(const std::string& name, const Color& value) const
{
	glUniform4f(UNIFORM_LOCATION, value.R, value.G, value.B, value.A);
}

void Shader::SetColor(Shader::Uniform uniform, const Color& value) const
{
	glUniform4f(uniform, value.R, value.G, value.B, value.A);
}

void Shader::SetMatrix4(const std::string& name, const Matrix4& value) const
{
	glUniformMatrix4fv(UNIFORM_LOCATION, 1, GL_FALSE, value.DataBlock());
}

void Shader::SetMatrix4(Shader::Uniform uniform, const Matrix4& value) const
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, value.DataBlock());
}

void Shader::CacheUniforms()
{
	m_cachedUniforms[(uint8)CachedUniform::Time] = GetUniformLocation("u_time");
	m_cachedUniforms[(uint8)CachedUniform::ScreenResolution] = GetUniformLocation("u_screenResolution");
	m_cachedUniforms[(uint8)CachedUniform::View] = GetUniformLocation("u_view");
	m_cachedUniforms[(uint8)CachedUniform::Projection] = GetUniformLocation("u_projection");
	m_cachedUniforms[(uint8)CachedUniform::Model] = GetUniformLocation("u_model");
	m_cachedUniforms[(uint8)CachedUniform::MVP] = GetUniformLocation("u_mvp");
	m_cachedUniforms[(uint8)CachedUniform::ViewProjection] = GetUniformLocation("u_viewProjection");
}
