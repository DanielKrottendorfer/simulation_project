#version 460

layout(location = 0) in vec4 Pos4D; 

void main() { 
    gl_Position = Pos4D;
}
