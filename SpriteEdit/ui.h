#pragma once

#include <sail-c++/sail-c++.h>
#include "common.h"

namespace ui
{
  class Image
  {
    sail::image m_image;

  public:
    void ReadNewImage(std::string_view file_path);
    void Save(std::string_view file_path);
    void SetPixel(int y, int x, ImVec4 color);

    unsigned int Width() const;
    unsigned int Height() const;
    const void* Pixels() const;
  };

  struct UIState
  {
    std::string m_open_file_path;
    Image m_current_image;

    ImVec4 m_primary_color{0.0f, 0.0f, 0.0f, 1.0f};
    ImVec4 m_secondary_color{1.0f, 1.0f, 1.0f, 1.0f};

    void Init(GLFWwindow* window);
    void DeInit();
    void Render();

  private:
    void OpenFile();
    void SaveFile();
  };
}
