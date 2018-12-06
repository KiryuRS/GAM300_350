#version 330 core

precision mediump float;
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 intexcoord;
layout (location = 3) in vec3 pos;
layout (location = 4) in vec2 size;
layout (location = 5) in float percentage;
layout (location = 6) in vec4 init_colour;
layout (location = 7) in vec4 final_colour;
layout (location = 8) in float init_alpha;
layout (location = 9) in float final_alpha;

out vec2 TexCoords;
out vec4 Ainit_colour;
out vec4 Afinal_colour;
out float Apercent;
out float Ainit_alpha;
out float Afinal_alpha;
uniform mat4 MVP;

void main(void) {
  mat4 trans = mat4(size.x * (1.0 - percentage) + size.y * percentage, 0, 0, 0, 
  0, size.x * (1.0 - percentage) + size.y * percentage, 0, 0, 
  0, 0, 1, 0, 
  pos.x, pos.y, pos.z, 1);
  gl_Position = MVP * trans * vec4(position, 1.0f);
  TexCoords = intexcoord;
  Ainit_colour = init_colour;
  Afinal_colour = final_colour;
  Ainit_alpha = init_alpha;
  Afinal_alpha = final_alpha;
  Apercent = percentage;
}