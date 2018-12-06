#version 330 core

precision mediump float;
out vec4 color;
in vec4 Ainit_colour;
in vec4 Afinal_colour;
in float Apercent;
in vec2 TexCoords;
in float Ainit_alpha;
in float Afinal_alpha;
uniform sampler2D diffuseMap;

void main(void) {
  vec4 texclr = texture(diffuseMap, TexCoords);
  
  vec4 mixcolor = mix(Ainit_colour, Afinal_colour, Apercent);
  float mixalpha = mix(Ainit_alpha, Afinal_alpha, Apercent);
  
  vec3 clr = mix(texclr, mixcolor, Ainit_colour.a).rgb;
  
  color = vec4(clr, texclr.a * mixalpha);
}
