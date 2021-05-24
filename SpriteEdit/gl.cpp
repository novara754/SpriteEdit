#include "gl.h"

namespace gl
{
  Shader::Shader(GLenum type, std::string_view source)
  {
    std::array sources{ source.data() };
    std::array lengths{ static_cast<GLint>(source.length()) };

    GLuint shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, sources.data(), lengths.data());
    glCompileShader(shader_id);

    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
      char info_log[512];
      GLsizei len;
      glGetShaderInfoLog(shader_id, 512, &len, info_log);
      spdlog::error("Failed to compile shader. Log: {}", info_log);

      assert(status == GL_TRUE);
    }

    m_id = shader_id;
    m_type = type;
  }
}
