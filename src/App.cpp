#include "App.h"

#include <iostream>
#include <imgui.h>
#include <memory>

#include "Renderer/Renderer.h"
#include "Renderer/VertexBufferLayout.h"

namespace App {

Renderer renderer;
std::unique_ptr<VertexBuffer> vb;
std::unique_ptr<VertexArray> vao;
std::unique_ptr<IndexBuffer> ib;
std::unique_ptr<Shader> shader;

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
  renderer.Draw(*vao, *ib, *shader);
}

void RenderInterface() {
  ImGui::Begin("Minha janelinha");
  ImGui::Text("Área de trabalho");
  if (ImGui::Button("Ativar")) {
    std::cout << "Botão de teste" << std::endl;
  }
  ImGui::End();
}

void Shutdown() {
  vb.release();
  vao.release();
  ib.release();
  shader.release();
}

}  // namespace App