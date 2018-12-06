#version 430 core

#define MAX_BONES 128

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 intexcoord;
layout (location = 3) in ivec4 bone_IDs;
layout (location = 4) in vec4 bone_weights;

out VS_OUT
{
  vec4 frag_pos;
  vec4 frag_normal;
  vec4 shadow_pos;
  vec2 uv;
} vs_out;

uniform mat4 MVP;
uniform mat4 World;
uniform mat4 invWorld;
uniform mat4 DepthBiasedMVP;
uniform mat4 gBones[MAX_BONES];
uniform int gNumBones;

void main(void) {
  mat4 BoneTransform = mat4(1.0f);  
  if (gNumBones != 0)
  {
    BoneTransform = gBones[bone_IDs[0]] * bone_weights[0];
    BoneTransform += gBones[bone_IDs[1]] * bone_weights[1];
    BoneTransform += gBones[bone_IDs[2]] * bone_weights[2];
    BoneTransform += gBones[bone_IDs[3]] * bone_weights[3];
  }
  
  vec4 localpos = BoneTransform * vec4(position, 1.0f);
  
  vs_out.frag_pos = World * localpos;
  vs_out.frag_normal = World * BoneTransform * vec4(normal, 0.0f);
  vs_out.uv = vec2(intexcoord.x, 1.0f - intexcoord.y);
  vs_out.shadow_pos = DepthBiasedMVP * localpos;
  
  gl_Position = MVP * localpos;
}