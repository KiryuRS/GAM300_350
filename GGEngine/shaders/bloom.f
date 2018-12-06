#version 330 core

out vec4 color;
out vec4 brightColor;
in vec4 vertexColor;
in vec2 TexCoords;
in vec3 FragPos;

struct Light {
  vec3 position;
  vec3 diffuse;
  
  float intensity;
  float range1;
};

#define MAX_LIGHTS 10

uniform vec2 extrauv;
uniform vec4 ambientLight;
uniform Light light[MAX_LIGHTS];
uniform int num_lights;
uniform float opacity;
uniform vec4 color_tint;

uniform sampler2D diffuseMap;

void main(void) {
	vec3 result;
	for (int i = 0; i < num_lights; ++i)
	  {
		vec3 lightDir = normalize(light[i].position - FragPos);
		float diff = max(dot(normal, lightDir), 0.0); 
		vec3 diffuse = light[i].diffuse * diff * texclr.rgb;
		
		  
		float d = length(light[i].position- FragPos);
		float denom = d / (light[i].range * 64.0) + 1.0;
		float attenuation = light[i].intensity / (denom * denom);
		
		// scale and bias the attenuation
		attenuation = (attenuation - 0.25) / (1.0 - 0.25);
		attenuation = max(attenuation, 0);
		
		result += (ambient + diffuse) * attenuation;
	  }
	color = vec4(result,1.0);
	float brightness = dot(color.rgb, vec3(0.21,0.72,0.07));
	if(brightness > 1.0)
		brightColor = vec4(color.rgb,1.0);
	else
		brightColor = vec4(0.0,0.0,0.0,0.0);
}