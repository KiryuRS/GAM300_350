#version 330 core

out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D image;
in vec2 TexCoords;
uniform float brightnessLimit;

void main(void) {
	FragColor = texture(image,TexCoords);
    float brightness = length(texture(image, TexCoords).rgb);
    if(brightness > brightnessLimit)
	//if(FragColor.x > brightnessLimit || FragColor.y > brightnessLimit || FragColor.z > brightnessLimit)
	{
		FragColor = vec4(texture(image, TexCoords).rgb, 1.0);
        BrightColor = vec4(texture(image, TexCoords).rgb, 1.0);
	}
    else
	{
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}