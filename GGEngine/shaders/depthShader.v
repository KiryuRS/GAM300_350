#version 430 core

#define MAX_BONES 128

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 intexcoord;
layout (location = 3) in ivec4 bone_IDs;
layout (location = 4) in vec4 bone_weights;

uniform mat4 DepthMVP;
uniform mat4 Model;
uniform mat4 gBones[MAX_BONES];
uniform int gNumBones;

void main(){

  mat4 BoneTransform = mat4(1.0f);  
  if (gNumBones != 0)
  {
    BoneTransform =  gBones[bone_IDs[0]] * bone_weights[0];
    BoneTransform += gBones[bone_IDs[1]] * bone_weights[1];
    BoneTransform += gBones[bone_IDs[2]] * bone_weights[2];
    BoneTransform += gBones[bone_IDs[3]] * bone_weights[3];
  }
  
  vec4 localpos = BoneTransform * vec4(pos, 1.0f);
  gl_Position =  DepthMVP * Model * localpos;
}