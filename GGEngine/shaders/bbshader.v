#version 330 core

layout (location = 0) in vec3 position; 
out vec4 vertexColor; 
uniform mat4 MVP;
uniform vec4 Color;

void main(void) {
  gl_Position = MVP * vec4(position, 1.0f);
  vertexColor = Color;
}