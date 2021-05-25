#pragma once

#include "common.h"

namespace ui
{
  struct UIState
  {
    ImVec4 m_primary_color{0.0f, 0.0f, 0.0f, 1.0f};
    ImVec4 m_secondary_color{1.0f, 1.0f, 1.0f, 1.0f};

    UIState(GLFWwindow* window);
    ~UIState();

    void Render();
    void OpenFile();
  };
}
