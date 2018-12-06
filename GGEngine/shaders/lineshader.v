#version 330

uniform mat4 model;
uniform mat4 projView;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec4 lineColor;

out VS_OUT
{
    vec4 rasterColor;
    vec4 vertexPosition;
    
} vs_out;

void main()
{
    vs_out.rasterColor = lineColor;
    vs_out.vertexPosition = projView * model *  vec4( modelPosition, 1.0f );
    gl_Position = vs_out.vertexPosition;
}
