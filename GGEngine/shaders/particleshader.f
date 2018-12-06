#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 emissive_mask;
in float Apercent;
in vec2 TexCoords;
in float Ainit_alpha;
in float Afinal_alpha;
uniform sampler2D diffuseMap;
uniform bool emissive;

void main(void) {
  vec4 texclr = texture(diffuseMap, TexCoords);
  
  float mixalpha = mix(Ainit_alpha, Afinal_alpha, Apercent);
  
  color = vec4(texclr.rgb, texclr.a * mixalpha);

  if (emissive)
	  emissive_mask = color;
  else
    emissive_mask = vec4(0, 0, 0, 1);
}
