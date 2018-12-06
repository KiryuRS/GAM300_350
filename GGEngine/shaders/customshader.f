#version 330 core

out vec4 color;

void main(void) {
  color = vec4(sin(gl_FragCoord.x*0.05)*0.5,
    cos(gl_FragCoord.y*0.25)*0.05,
    sin(gl_FragCoord.x*0.15)*cos(gl_FragCoord.y*0.15), 1.0);
}