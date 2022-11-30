#pragma once

#include "Core/Base.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Math/Matrix4.h"
#include <unordered_map>
#include <array>

class GARBAGE_API Shader final
{
public:

    using Uniform = uint32;

    enum class Type
    {
        Vertex, Fragment, Geometry
    };

    enum class CachedUniform
    {
        Time, ScreenResolution, View, Projection, Model, MVP, ViewProjection
    };

    Shader(const std::unordered_map<Type, std::string_view>& sources, const std::unordered_map<std::string_view, std::string>& parameters = std::unordered_map<std::string_view, std::string>());
    ~Shader();

    void Bind();
    void Unbind();

    Uniform GetUniformLocation(std::string_view name) const;
    Uniform GetUniformLocation(CachedUniform uniform) const;

    void SetBool(const std::string& name, bool value) const;
    void SetBool(Uniform uniform, bool value) const;
    void SetInt32(const std::string& name, int32 value) const;
    void SetInt32(Uniform uniform, int32 value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat(Uniform uniform, float value) const;
    void SetVector2(const std::string& name, const Vector2& value) const;
    void SetVector2(Uniform uniform, const Vector2& value) const;
    void SetVector3(const std::string& name, const Vector3& value) const;
    void SetVector3(Uniform uniform, const Vector3& value) const;
    void SetVector4(const std::string& name, const Vector4& value) const;
    void SetVector4(Uniform uniform, const Vector4& value) const;
    void SetColor(const std::string& name, const Color& value) const;
    void SetColor(Uniform uniform, const Color& value) const;
    void SetMatrix4(const std::string& name, const Matrix4& value) const;
    void SetMatrix4(Uniform uniform, const Matrix4& value) const;

    bool IsValid() const { return m_id != 0; }

private:

    uint32 m_id;

    std::array<Uniform, 32> m_cachedUniforms;

    void CacheUniforms();

};