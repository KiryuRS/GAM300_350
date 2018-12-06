#version 430 core

out vec4 color;

uniform float Gamma;
uniform sampler2D screen_image;
in vec2 TexCoords;

void main(void) {
  float invgamma = 1.0 / Gamma;
	color = pow(texture(screen_image,TexCoords), vec4(invgamma, invgamma, invgamma, invgamma));
}
