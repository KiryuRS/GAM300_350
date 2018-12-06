#version 330 core

out vec4 color;
in vec4 vertexColor;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D scene;
uniform sampler2D blur;
uniform bool bloom;
uniform float exposure;

void main(void) {
	const float gamma = 2.2;
	vec3 hdrColor = texture(scene, TexCoords).rgb;
	vec3 bloomColor = texture(blur,TexCoords).rgb;
	if(bloom)
		hdrColor += bloomColor;
	
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	
	result = pow(result, vec3(1.0 / gamma));
	color = vec4(result,1.0);
}