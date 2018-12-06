#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 emissive_mask;
in vec2 TexCoords;
in vec4 FragPos;
in vec4 Center;
in vec3 ModelPos;

// 1 for clock type
// 2 for horizontal type
// 3 for vertical type 
uniform int Mode;
uniform sampler2D Texture;

uniform float Value; // the angle to start clock type 
uniform float startSize;  // the position to start h/v 
uniform float endSize;    // the position to end h/v
uniform float m_global_transparency;
uniform vec4 m_tint;

#define PI 3.1415926535f

uniform sampler2D image;

void main(void) {
  vec3 vec = ModelPos;

  if (Mode == 0)
  {
    color = texture(Texture, TexCoords);
  }
  else if (Mode == 1) // clock type
  {
    float angle = atan(vec.y / vec.x);

    if (angle < 0)
      angle += 3.1415926f;


    if (vec.y < 0)
      angle += PI;


    if (angle > Value)
      color = texture(Texture, TexCoords);
    else
      color = vec4(1.0f, 0.0f, 0.0f, 0.0f);

  }
  else if (Mode == 2) // horizontal type
  {
    if ((ModelPos.x - startSize) / (endSize - startSize) >= Value)
      color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    else
      color = texture(Texture, TexCoords);
  }
  else if (Mode == 3) // vertical type
  {
    if ((ModelPos.y - startSize) / (endSize - startSize) >= Value)
      color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    else
      color = texture(Texture, TexCoords);
  }
  vec3 col = mix(color.rgb, m_tint.rgb, m_tint.a);
  color.rgb = col;
	color.w *= m_global_transparency;
  
	emissive_mask = vec4 (0.0f, 0.0f, 0.0f, 1.0f);

  if (color.w < 0.001f)
    discard;
}