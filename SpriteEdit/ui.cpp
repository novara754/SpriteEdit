#include "ui.h"

#include <Windows.h>
#include <commdlg.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ui
{
  UIState::UIState(GLFWwindow* window)
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();
  }

  UIState::~UIState()
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
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Color Picker");
    ImGui::Text("Primary Color:");
    ImGui::ColorEdit3("", reinterpret_cast<float*>(&m_primary_color));
    ImGui::Text("Secondary Color:");
    ImGui::ColorEdit3("", reinterpret_cast<float*>(&m_secondary_color));
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
      sail::image_writer writer;
      sail::image_reader reader;
      reader.read(file_path, &m_current_image);
      m_current_image.convert(SAIL_PIXEL_FORMAT_BPP24_RGB);

      glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        m_current_image.width(),
        m_current_image.height(),
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        m_current_image.pixels()
      );
    }
  }
}
