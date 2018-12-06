#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 intexcoord;
layout (location = 3) in vec3 pos;
layout (location = 4) in float init_size;
layout (location = 5) in float final_size;
layout (location = 6) in float percentage;
layout (location = 7) in float init_alpha;
layout (location = 8) in float final_alpha;
layout (location = 9) in vec3  rotation;

out vec2 TexCoords;
out float Apercent;
out float Ainit_alpha;
out float Afinal_alpha;
uniform mat4 VP;
uniform mat4 Model;

void main(void) {
  float csize = init_size * (1.0 - percentage) + final_size * percentage;
  mat4 scale = mat4(
     csize, 0, 0, 0, 
     0, csize, 0, 0, 
     0, 0, csize, 0, 
     0, 0, 0, 1);
  mat4 trans = mat4(1, 0, 0, 0, 
     0, 1, 0, 0, 
     0, 0, 1, 0, 
     pos.x, pos.y, pos.z, 1);
  
  float sinx = sin(rotation.x);
  float cosx = cos(rotation.x);
  float siny = sin(rotation.y);
  float cosy = cos(rotation.y);
  float sinz = sin(rotation.z);
  float cosz = cos(rotation.z);

  mat4 rotX = mat4(
    cosx, sinx, 0, 0,
    -sinx, cosx, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);
  mat4 rotY = mat4(
    cosy, 0, siny, 0,
    0, 1, 0, 0,
    -siny, 0, cosy, 0,
    0, 0, 0, 1);
  mat4 rotZ = mat4(
    1, 0, 0, 0,
    0, cosz, sinz, 0,
    0, -sinz, cosz, 0,
    0, 0, 0, 1);

  mat4 finalrotation = rotX * rotY * rotZ;

  gl_Position = VP * trans * Model * finalrotation  * scale * vec4(position, 1.0f);
  TexCoords = intexcoord;
  Ainit_alpha = init_alpha;
  Afinal_alpha = final_alpha;
  Apercent = percentage;
}