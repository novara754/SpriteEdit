#pragma once

// C++ standard library
#include <array>
#include <string>
#include <string_view>

// Math
#pragma warning(push)
#pragma warning(disable: 4201) // Disable warnings about nameless structs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)

// UI
#include <imgui.h>

// Spdlog
#include <spdlog/spdlog.h>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
