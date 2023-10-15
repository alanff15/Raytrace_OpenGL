#include "App.h"

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Renderer/Renderer.h"
#include "Renderer/VertexBufferLayout.h"

#include <memory>

#include "../../res/app.glsl"

#ifndef M_PI
#define M_PI ((float)3.14159265358979323846)
#endif

namespace App {

Renderer renderer;
std::unique_ptr<VertexBuffer> vb;
std::unique_ptr<VertexArray> vao;
std::unique_ptr<IndexBuffer> ib;
std::unique_ptr<Shader> shader;

float FOV = 1.0;
glm::vec3 angles(30.0f * M_PI / 180.0f, 0, 0);
float metalness = 0.1;
float roughness = 0.0;

void Setup() {
  // blend
  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  // vertices
  float data[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
  vao = std::make_unique<VertexArray>();
  vb = std::make_unique<VertexBuffer>(data, 4 * 2 * sizeof(float));
  VertexBufferLayout vbl;
  vbl.Push<float>(2);
  vao->AddBuffer(*vb, vbl);
  // indices
  uint32_t indices[] = {0, 1, 2, 2, 3, 0};
  ib = std::make_unique<IndexBuffer>(indices, 6);
  // shader
  // shader = std::make_unique<Shader>("../../res/app.glsl", StringType::FILEPATH);
  shader = std::make_unique<Shader>(GLSL_STR, StringType::PROGRAM);
  shader->Bind();
}

glm::mat3 rotationMatrixX(float angle) {
  return glm::mat3(1, 0, 0, 0, cos(angle), -sin(angle), 0, sin(angle), cos(angle));
}

glm::mat3 rotationMatrixY(float angle) {
  return glm::mat3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle));
}

glm::mat3 rotationMatrixZ(float angle) {
  return glm::mat3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
}

void Render(GLFWwindow* window) {
  int w, h;
  glfwGetWindowSize(window, &w, &h);
  // rotação da câmera
  glm::mat3 matRot = rotationMatrixZ(angles.z) * rotationMatrixX(angles.x) * rotationMatrixY(angles.y);
  // posição e orientação da câmera em pixels antes da rotação (sobre o eixo y apontando para a origem)
  glm::vec3 camera_pos = matRot * glm::vec3(0.0, -15.0, 0.0) + glm::vec3(0.0, 0.0, 2.0);
  glm::mat3 camera_mat = matRot * glm::mat3(/*vx*/ 1.0, 0.0, 0.0, /*vy*/ 0.0, 0.0, -1.0, /*vz*/ 0.0, 1.0, 0.0);

  // set uniforms
  shader->Bind();
  shader->SetUniform2f("u_ScreenResolution", (float)w, (float)h);
  shader->SetUniform1f("u_CameraFOV", FOV);
  shader->SetUniform1f("u_Metalness", metalness);
  shader->SetUniform1f("u_Roughness", roughness);
  shader->SetUniform3f("u_CameraPos", camera_pos.x, camera_pos.y, camera_pos.z);
  shader->SetUniformMat3f("u_CameraMat", camera_mat);
  renderer.Draw(*vao, *ib, *shader);
}

void RenderInterface() {
  // ImGui::GetIO()->NativePtr->IniFilename = "my_config.ini";
  ImGui::GetIO().IniFilename = NULL;
  ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x - 60, ImGui::GetWindowPos().y - 60), ImGuiCond_::ImGuiCond_None);
  ImGui::SetNextWindowSize(ImVec2(420, 100), ImGuiCond_::ImGuiCond_None);
  ImGui::SetNextWindowBgAlpha(0.25f);
  ImGui::Begin("Câmera");
  ImGui::Text("FPS: %3.1f", (ImGui::GetIO()).Framerate);
  // ImGui::SliderFloat("FOV", &FOV, 0.5f, 2.0f);
  // ImGui::SliderFloat("Ângulo X", &angles.x, -M_PI / 2, M_PI / 2);
  // ImGui::SliderFloat("Ângulo Z", &angles.z, -M_PI, M_PI);
  ImGui::SliderFloat("Metalness", &metalness, 0, 1);
  ImGui::SliderFloat("Roughness", &roughness, 0, 1);
  ImGui::End();
}

void Shutdown() {
  vb.release();
  vao.release();
  ib.release();
  shader.release();
}

// mouse events
bool mousePressedB1(false);
glm::dvec2 mouseStartB1;
glm::vec3 startAngles;

void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
  switch (button) {
    case GLFW_MOUSE_BUTTON_1:
      mousePressedB1 = false;
      if (action == 1) {
        glfwGetCursorPos(window, &mouseStartB1.x, &mouseStartB1.y);
        if (mouseStartB1.y > (100 + 4) || mouseStartB1.x > (400 + 23)) {
          startAngles = angles;
          mousePressedB1 = true;
        }
      }
      break;
  }
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);  // imgui
}

void cursorPosEvent(GLFWwindow* window, double xpos, double ypos) {
  if (mousePressedB1) {
    glm::dvec2 mousePos;
    glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
    glm::dvec2 mouseDelta = mousePos - mouseStartB1;
    angles.x = startAngles.x + (float)mouseDelta.y / 400.f;
    angles.z = startAngles.z + (float)mouseDelta.x / 400.f;
  }
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);  // imgui
}

void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, 1);
      break;
  }
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);  // imgui
}

}  // namespace App