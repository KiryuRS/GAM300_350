#version 430 core


layout(location = 0) out vec4 color;
layout(location = 1) out vec4 emissive_mask;


in VS_OUT
{
    vec4 rasterColor;
    vec4 vertexPosition;
    
} fs_in;


void main()
{
    color = fs_in.rasterColor;
    emissive_mask = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
