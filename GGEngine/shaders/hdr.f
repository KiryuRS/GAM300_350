#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D color1;
uniform sampler2D color2;
uniform bool hdr;
uniform float exposure;
uniform bool bloom;

uniform float weights[5] = float[](0.227,0.194,0.121,0.054,0.016);

void main(void) {
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
      if(bloom)
        hdrColor += texture(color1, TexCoords).rgb;
	//if(hdrColor == 0)
	//hdrColor = vec3(1.0,0.0,1.0);
    if(hdr)
    {
      //vec3 hdrColor2 = texture(hdrBuffer, TexCoords).rgb + texture(color1, TexCoords).rgb;
      vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
      //vec3 result = hdrColor / (hdrColor + vec3(1.0));
      //float gamma = 2.2f;
      //    result = pow(result, vec3(1.0 / gamma));
      
      //hdrColor2 = vec3(1.0) - exp(-hdrColor2 * exposure);
          //hdrColor2 = pow(hdrColor2, vec3(1.0 / gamma));
      //if(TexCoords.x < 0.5f)
        //color = vec4(hdrColor2, 1.0);
      //else	
        //color = vec4(texture(color1, TexCoords).rgb, 1.0);
      color = vec4(result, 1.0);
    }
    else
    {
        vec3 result = hdrColor; 
        color = vec4(result, 1.0);
    }
	
    //color = texture(hdrBuffer, TexCoords);
	//color = vec4(exposure,TexCoords,1.0);
}