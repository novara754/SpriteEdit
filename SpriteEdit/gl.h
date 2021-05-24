#pragma once

#include "common.h"

namespace gl
{
  template<typename T, std::size_t N>
  void BufferData(GLenum target, std::array<T, N> data, GLenum usage)
  {
    glBufferData(target, sizeof(T) * N, data.data(), usage);
  }

  struct VertexArray
  {
    GLuint m_id;

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray()
    {
      glGenVertexArrays(1, &m_id);
    }

    void Bind()
    {
      glBindVertexArray(m_id);
    }

    ~VertexArray()
    {
      glDeleteVertexArrays(1, &m_id);
    }
  };

  struct Buffer
  {
    GLuint m_id;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer()
    {
      glGenBuffers(1, &m_id);
    }

    void Bind(GLenum target)
    {
      glBindBuffer(target, m_id);
    }

    ~Buffer()
    {
      glDeleteBuffers(1, &m_id);
    }
  };

  struct Shader
  {
    GLuint m_id;
    GLenum m_type;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(GLenum type, std::string_view source);

    ~Shader()
    {
      glDeleteShader(m_id);
    }
  };

  struct ShaderAttribute
  {
    GLint m_location;

    ShaderAttribute(GLint location)
      : m_location(location)
    {}

    void SetPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLint offset)
    {
      spdlog::info("{} == {}?", sizeof(GLint), sizeof(void*));

      glVertexAttribPointer(
        static_cast<GLuint>(m_location),
        size,
        type,
        normalized,
        stride,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(offset))
      );
    }

    void Enable()
    {
      glEnableVertexAttribArray(static_cast<GLuint>(m_location));
    }
  };

  struct ShaderUniform
  {
    GLint m_location;

    ShaderUniform(GLint location)
      : m_location(location)
    {}

    void Matrix4fv(GLsizei count, GLboolean transpose, GLfloat *data)
    {
      glUniformMatrix4fv(m_location, count, transpose, data);
    }
  };

  struct ShaderProgram
  {
    GLuint m_id;

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram()
    {
      m_id = glCreateProgram();
      assert(m_id != 0);
    }

    void AttachShader(const Shader& shader)
    {
      glAttachShader(m_id, shader.m_id);
    }

    void BindFragDataLocation(GLuint color_number, std::string_view name)
    {
      glBindFragDataLocation(m_id, color_number, name.data());
    }

    void Link()
    {
      glLinkProgram(m_id);
    }

    void Use()
    {
      glUseProgram(m_id);
    }

    ShaderAttribute GetAttribLocation(std::string_view name)
    {
      return ShaderAttribute(glGetAttribLocation(m_id, name.data()));
    }

    ShaderUniform GetUniformLocation(std::string_view name)
    {
      return ShaderUniform(glGetUniformLocation(m_id, name.data()));
    }
  };
}
