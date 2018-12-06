#version 330 core

out vec4 color;
in vec2 TexCoords;

uniform bool horizontal;
//blur weights
uniform float weights[5] = float[](0.227,0.194,0.121,0.054,0.016);

uniform sampler2D image;

void main(void) {
	vec2 texOffset = 1.0 / textureSize(image,0);
	vec3 result = texture(image, TexCoords).rgb * weights[0];
	if(horizontal)
		for(int i = 1; i < 5; ++i)
		{
			result += texture(image, TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weights[i];
			result += texture(image, TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weights[i];
		}
	else
		for(int i = 1; i < 5; ++i)
		{
			result += texture(image, TexCoords + vec2(0.0,texOffset.y * i)).rgb * weights[i];
			result += texture(image, TexCoords - vec2(0.0,texOffset.y * i)).rgb * weights[i];
		}
	//color = vec4(texture(image, TexCoords).rgb,1.0);
	color = vec4(result,1.0);
}