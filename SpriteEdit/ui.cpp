#include "ui.h"

#include <Windows.h>
#include <commdlg.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ui
{
  void UIState::Init(GLFWwindow* window)
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();
  }

  void UIState::DeInit()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void UIState::Render()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Open...", "Ctrl+O"))
          OpenFile();
        if (ImGui::MenuItem("Save", "Ctrl+S"))
          SaveFile();
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Color Picker");
    ImGui::ColorEdit4("Primary Color", reinterpret_cast<float*>(&m_primary_color));
    ImGui::ColorEdit4("Secondary Color", reinterpret_cast<float*>(&m_secondary_color));
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void UIState::OpenFile()
  {
    char file_path[MAX_PATH]{};
    OPENFILENAMEA open_file_name{};
    open_file_name.lStructSize = sizeof(open_file_name);
    open_file_name.hInstance = GetModuleHandle(nullptr);
    open_file_name.hwndOwner = GetActiveWindow();
    open_file_name.lpstrTitle = "Open...";
    open_file_name.lpstrFilter = "PNG Files (*.png)\0*.png\0All Files (*.*)\0*.*\0";
    open_file_name.nMaxFile = MAX_PATH;
    open_file_name.lpstrFile = file_path;
    if (GetOpenFileNameA(&open_file_name))
    {
      m_open_file_path = file_path;
      m_current_image.ReadNewImage(file_path);

      glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        m_current_image.Width(),
        m_current_image.Height(),
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        m_current_image.Pixels()
      );
    }
  }

  void UIState::SaveFile()
  {
    m_current_image.Save(m_open_file_path);
  }

  void Image::ReadNewImage(std::string_view file_path)
  {
    sail::image_writer writer;
    sail::image_reader reader;
    reader.read(file_path, &m_image);
    m_image.convert(SAIL_PIXEL_FORMAT_BPP24_RGB);
  }

  void Image::Save(std::string_view file_path)
  {
    sail::image_writer writer;
    writer.write(file_path, m_image);
  }

  void Image::SetPixel(int y, int x, ImVec4 color)
  {
    std::array new_pixel{
      static_cast<std::uint8_t>(color.x * 255), // R
      static_cast<std::uint8_t>(color.y * 255), // G
      static_cast<std::uint8_t>(color.z * 255), // B
    };

    spdlog::debug("{}, {}, {}", new_pixel[0], new_pixel[1], new_pixel[2]);

    auto* pixels = static_cast<std::uint8_t*>(m_image.pixels());
    auto idx = 3 * (y * Width() + x);
    std::copy(new_pixel.begin(), new_pixel.end(), &pixels[idx]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, new_pixel.data());
  }

  unsigned int Image::Width() const
  {
    return m_image.width();
  }

  unsigned int Image::Height() const
  {
    return m_image.height();
  }

  const void* Image::Pixels() const
  {
    return m_image.pixels();
  }
}
