#version 430 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

uniform mat4 MVP;
uniform mat4 VWorld;

out vec4 FragPos;
out vec4 Center;
out vec3 ModelPos;


void main(void) {
  TexCoords = texCoord;
  gl_Position = MVP * vec4(position, 1.0f);


  Center = VWorld * vec4(0.0f, 0.0f, 0.0f, 1.0f);
  FragPos = VWorld * vec4(position, 1.0f);
  ModelPos = position;
}