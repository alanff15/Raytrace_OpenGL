#include "App.h"

#include <iostream>
#include <imgui.h>
#include <memory>

#include "Renderer/Renderer.h"
#include "Renderer/VertexBufferLayout.h"

#ifndef M_PI
#define M_PI ((float)3.14159265358979323846)
#endif

namespace App {

Renderer renderer;
std::unique_ptr<VertexBuffer> vb;
std::unique_ptr<VertexArray> vao;
std::unique_ptr<IndexBuffer> ib;
std::unique_ptr<Shader> shader;

float angles[3] = {30.0f * M_PI / 180.0f, 0, 0};

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
  shader = std::make_unique<Shader>("../../res/app.glsl");
  shader->Bind();
}

void Render() {
  shader->Bind();
  shader->SetUniform3f("u_DirectorAngles", angles[0], angles[1], angles[2]);
  renderer.Draw(*vao, *ib, *shader);
}

void RenderInterface() {
  ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x - 60, ImGui::GetWindowPos().y - 60), ImGuiCond_::ImGuiCond_None);
  ImGui::SetNextWindowSize(ImVec2(420, 80), ImGuiCond_::ImGuiCond_None);
  ImGui::SetNextWindowBgAlpha(0.25f);
  ImGui::Begin("Câmera");
  ImGui::SliderFloat("Ângulo X", &angles[0], -M_PI / 2, M_PI / 2);
  ImGui::SliderFloat("Ângulo Z", &angles[2], -M_PI, M_PI);
  ImGui::End();
}

void Shutdown() {
  vb.release();
  vao.release();
  ib.release();
  shader.release();
}

}  // namespace App