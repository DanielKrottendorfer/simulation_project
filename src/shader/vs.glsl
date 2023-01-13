#version 460

layout(location = 0) in vec4 Pos4D; 
layout(location = 1) in vec4 coloress;

layout(location = 0) uniform mat4 proj;

out vec4 colD;

void main() { 
    colD = coloress;
    gl_Position = proj * Pos4D;
}
