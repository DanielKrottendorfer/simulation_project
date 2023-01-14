#version 460

layout(location = 0) in vec4 Pos4D; 
layout(location = 1) in vec4 colors;


void main() { 
    gl_Position = Pos4D;
}
