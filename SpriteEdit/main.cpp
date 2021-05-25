#include <iostream>
#include "common.h"
#include "gl.h"
#include "ui.h"

constexpr int DEF_WIN_WIDTH = 800;
constexpr int DEF_WIN_HEIGHT = 600;
constexpr std::string_view WIN_TITLE = "SpriteEdit";
const ImVec4 BACKGROUND_COLOR(0.2f, 0.2f, 0.2f, 1.0f);

constexpr const char* VERTEX_SHADER_SOURCE = R"glsl(
  #version 150 core
  
  in vec2 position;
  in vec2 in_tex_coord;

  out vec2 tex_coord;

  uniform mat4 scale;
  uniform mat4 zoom;

  void main()
  {
    tex_coord = in_tex_coord;
    gl_Position = zoom * scale * vec4(position, 0.0, 1.0);
  }
)glsl";

constexpr const char* FRAGMENT_SHADER_SOURCE = R"glsl(
  #version 150 core

  in vec2 tex_coord;

  out vec4 out_color;

  uniform sampler2D image_texture;

  void main()
  {
    out_color = texture(image_texture, tex_coord);
  }
)glsl";

ui::UIState ui_state;
float zoom_factor = 1.0f;

std::pair<float, float> CalculateImageScaling(int win_width, int win_height)
{
  // The following code adjusts the scale of the image rendering plane
  // to fit the currently loaded image and also adjusts the scale based on the 
  // aspect ratio of the window.
  //
  // First the window's aspect ratio:
  // If the window is wider than it is high `win_width_scale` will be between `0.0` and `1.0`
  // this way the image rendering plane will be shrunk horizontally to adjust for the stretching of the window.
  // The same thing applies to `win_height_scale` in the vertical direction.
  auto win_width_f = static_cast<float>(win_width);
  auto win_height_f = static_cast<float>(win_height);
  auto win_width_scale = win_height_f / std::max(win_width_f, win_height_f);
  auto win_height_scale = win_width_f / std::max(win_width_f, win_height_f);
  // Now the image's aspect ratio:
  // In order for the image's bigger dimension to span the entire range of `(-1.0, 1.0)` in OpenGL coordinates
  // the image rendering pane will be shrunk in the direction of the image's smaller dimension.
  // For example:
  // When the image is wider than it is high the X-coordinates will span from `-1.0` to `1.0` because
  // `image_width_scale` will be `1.0`. Meanwhile `image_height_scale` will end up between `0.0` and `1.0`
  // and thus the image rendering plane will be shrunk in the vertical direction to maintain the right aspect ratio.
  auto image_width = static_cast<float>(ui_state.m_current_image.Width());
  auto image_height = static_cast<float>(ui_state.m_current_image.Height());
  auto image_width_scale = image_width / std::max(image_width, image_height);
  auto image_height_scale = image_height / std::max(image_width, image_height);

  return {win_width_scale * image_width_scale, win_height_scale * image_height_scale};
}

void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
  (void)x_offset;

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    if (y_offset > 0.0)
      zoom_factor *= 1.1f;
    else
      zoom_factor *= 0.9f;

    if (zoom_factor > 60.0f)
      zoom_factor = 60.0f;
  }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (action == GLFW_PRESS && (button == GLFW_MOUSE_BUTTON_LEFT || button == button == GLFW_MOUSE_BUTTON_RIGHT))
  {
    ImVec4 active_color = button == GLFW_MOUSE_BUTTON_LEFT
      ? ui_state.m_primary_color
      : ui_state.m_secondary_color;
    
    int win_width, win_height;
    glfwGetFramebufferSize(window, &win_width, &win_height);
    
    auto [width_scale, height_scale] = CalculateImageScaling(win_width, win_height);

    // The width and height scale determine how much of the window the rendered image will take up.
    // Using this information and the actual size of the window you can easily determine the size of the rendered
    // image on the screen ("image area").
    auto image_area_width = win_width * width_scale * zoom_factor;
    auto image_area_height = win_height * height_scale * zoom_factor;

    // The rendered image is centered around the middle of the window.
    // From this information you can calculate the upper left corner of the image area as well.
    auto image_area_left = (win_width - image_area_width) / 2.0;
    auto image_area_top = (win_height - image_area_height) / 2.0;

    double cursor_x, cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);

    // The position upper left corner, the width and the heigh perfectly define the rectangle
    // of the image area. Now you can check whether the mouse cursor is inside of this rectangle.
    // If it is, that means the user clicked somewhere on the rendered image!
    if (cursor_x >= image_area_left && cursor_x <= (image_area_left + image_area_width)
      && cursor_y >= image_area_top && cursor_y <= (image_area_top + image_area_height))
    {
      auto image_width = ui_state.m_current_image.Width();
      auto image_height = ui_state.m_current_image.Height();

      // The distance of the cursor from the top left corner of the image area can be used to calculate
      // *where* the user clicked. This coordinate then needs to be converted into coordinates on the image itself.
      auto x_image = static_cast<int>((cursor_x - image_area_left) / image_area_width * image_width);
      auto y_image = static_cast<int>((cursor_y - image_area_top) / image_area_height * image_height);

      ui_state.m_current_image.SetPixel(y_image, x_image, active_color);
    }
  }
}

int main()
{
  spdlog::set_level(spdlog::level::debug);
  sail::log::set_barrier(SAIL_LOG_LEVEL_SILENCE);

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
  glfwSetMouseButtonCallback(window, MouseButtonCallback);

  glewInit();

  gl::VertexArray vao;
  vao.Bind();

  std::array<GLfloat, 20> vertices{
    -1.0f,  1.0f, 0.0f, 0.0f, // Top left
     1.0f,  1.0f, 1.0f, 0.0f, // Top right
     1.0f, -1.0f, 1.0f, 1.0f, // Bottom right
    -1.0f, -1.0f, 0.0f, 1.0f, // Bottom left
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

  gl::Texture image_texture;
  image_texture.Bind(GL_TEXTURE_2D);
  program.GetUniformLocation("image_texture").Uniform1i(0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  auto positionAttribute = program.GetAttribLocation("position");
  positionAttribute.SetPointer(2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  positionAttribute.Enable();

  auto colorAttribute = program.GetAttribLocation("in_tex_coord");
  colorAttribute.SetPointer(2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 2 * sizeof(GLfloat));
  colorAttribute.Enable();

  auto scaleUniform = program.GetUniformLocation("scale");
  auto zoomUniform = program.GetUniformLocation("zoom");

  ui_state.Init(window);

  ImVec4 primary_color(0.0f, 0.0f, 0.0f, 1.0f);
  ImVec4 secondary_color(1.0f, 1.0f, 1.0f, 1.0f);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    int win_width, win_height;
    glfwGetFramebufferSize(window, &win_width, &win_height);
    glViewport(0, 0, win_width, win_height);

    glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
    glClear(GL_COLOR_BUFFER_BIT);

    auto [width_scale, height_scale] = CalculateImageScaling(win_width, win_height);
    auto scale = glm::scale(glm::vec3(
      width_scale,
      height_scale,
      1.0f
    ));
    scaleUniform.Matrix4fv(1, GL_FALSE, glm::value_ptr(scale));

    auto zoom = glm::scale(glm::vec3(zoom_factor, zoom_factor, 1.0f));
    zoomUniform.Matrix4fv(1, GL_FALSE, glm::value_ptr(zoom));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ui_state.Render();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}
