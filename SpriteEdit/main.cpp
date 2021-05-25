#include <iostream>
#include "common.h"
#include "gl.h"
#include "ui.h"

constexpr int DEF_WIN_WIDTH = 800;
constexpr int DEF_WIN_HEIGHT = 600;
constexpr std::string_view WIN_TITLE = "SpriteEdit";
const ImVec4 BACKGROUND_COLOR(0.2f, 0.2f, 0.2f, 1.0f);

constexpr const char * VERTEX_SHADER_SOURCE = R"glsl(
  #version 150 core
  
  in vec2 position;
  in vec3 in_color;

  out vec3 color;

  uniform mat4 zoom;

  void main()
  {
    color = in_color;
    gl_Position = zoom * vec4(position, 0.0, 1.0);
  }
)glsl";

constexpr const char * FRAGMENT_SHADER_SOURCE = R"glsl(
  #version 150 core

  in vec3 color;

  out vec4 out_color;

  void main()
  {
    out_color = vec4(color, 1.0);
  }
)glsl";

float scale = 0.8f;

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
  (void)x_offset;
  
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    if (y_offset > 0.0)
      scale *= 1.1f;
    else
      scale *= 0.9f;

    if (scale > 60.0f)
      scale = 60.0f;
  }
}

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  auto* window = glfwCreateWindow(DEF_WIN_WIDTH, DEF_WIN_HEIGHT, WIN_TITLE.data(), nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetScrollCallback(window, ScrollCallback);

  glewInit();

  gl::VertexArray vao;
  vao.Bind();

  std::array<GLfloat, 20> vertices{
     1.0f,  1.0f, 1.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,
  };
  gl::Buffer vbo;
  vbo.Bind(GL_ARRAY_BUFFER);
  gl::BufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW);

  std::array<GLuint, 6> elements{
    0, 1, 2,
    2, 3, 0,
  };
  gl::Buffer ebo;
  ebo.Bind(GL_ELEMENT_ARRAY_BUFFER);
  gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, elements, GL_STATIC_DRAW);

  gl::Shader vertex_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
  gl::Shader fragment_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
  gl::ShaderProgram program;
  program.AttachShader(vertex_shader);
  program.AttachShader(fragment_shader);
  program.BindFragDataLocation(0, "out_color");
  program.Link();
  program.Use();

  auto positionAttribute = program.GetAttribLocation("position");
  positionAttribute.SetPointer(2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
  positionAttribute.Enable();

  auto colorAttribute = program.GetAttribLocation("in_color");
  colorAttribute.SetPointer(3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 2 * sizeof(GLfloat));
  colorAttribute.Enable();

  auto zoomUniform = program.GetUniformLocation("zoom");

  ui::UIState ui_state(window);

  ImVec4 primary_color(0.0f, 0.0f, 0.0f, 1.0f);
  ImVec4 secondary_color(1.0f, 1.0f, 1.0f, 1.0f);

  while (!glfwWindowShouldClose(window))
  {
    int win_width, win_height;
    glfwGetWindowSize(window, &win_width, &win_height);

    glfwPollEvents();
    glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
    glClear(GL_COLOR_BUFFER_BIT);

    auto zoom = glm::scale(glm::vec3(scale, scale, 1.0f));
    zoomUniform.Matrix4fv(1, GL_FALSE, glm::value_ptr(zoom));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ui_state.Render();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}
