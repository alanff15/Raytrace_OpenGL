#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};

class Shader {
private:
  std::string m_FilePath;
  uint32_t m_RendererID;
  std::unordered_map<std::string, int> m_UniformLocationCahe;

public:
  Shader(const std::string& filepath);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  // Set uniforms
  void SetUniform1i(const std::string& name, int value);
  void SetUniform2i(const std::string& name, int v0, int v1);
  void SetUniform1f(const std::string& name, float value);
  void SetUniform2f(const std::string& name, float v0, float v1);
  void SetUniform3f(const std::string& name, float v0, float v1, float v2);
  void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
  void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
  ShaderProgramSource parseShader(const std::string& filepath);
  uint32_t CompileShaders(uint32_t type, const std::string& source);
  int GetUniformLocation(const std::string& name);

  uint32_t CreateShaders(const std::string& vertexShader, const std::string& fragmentShader);
};