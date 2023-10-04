#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
out vec2 FragCoord;

void main() {
    gl_Position = position;
    FragCoord = position.xy;
}

#-----------------------------------------------------------#

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 FragCoord;

void main() {
    color = vec4(FragCoord.x * 0.5 + 0.5, FragCoord.y * 0.5 + 0.5, 0.0, 1.0);
}