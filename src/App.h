#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace App {
void Setup(GLFWwindow* window);
void Render(GLFWwindow* window);
void RenderInterface(GLFWwindow* window);
void Shutdown(GLFWwindow* window);

void cursorPosEvent(GLFWwindow* window, double xpos, double ypos);
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
}  // namespace App