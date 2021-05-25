#include "ui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ui
{
  UIState::UIState(GLFWwindow *window)
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
          spdlog::info("'Open...' was clicked");
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
}
