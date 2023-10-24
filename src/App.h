#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace App {

void Setup(GLFWwindow* window);
void Render(GLFWwindow* window);
void RenderInterface(GLFWwindow* window);
void Shutdown(GLFWwindow* window);

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

}  // namespace App