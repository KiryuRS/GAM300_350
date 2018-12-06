#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 intexcoord;
out vec4 vertexColor;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 Model;

void main(void) {
  gl_Position = MVP * vec4(position, 1.0f);
  vertexColor = color;
  TexCoords = intexcoord;
  FragPos = vec3(Model * vec4(position, 1.0f));
}