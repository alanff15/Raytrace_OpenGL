#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace App {
void Setup();
void Render(GLFWwindow* window);
void RenderInterface();
void Shutdown();

void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
void cursorPosEvent(GLFWwindow* window, double xpos, double ypos);
}  // namespace App