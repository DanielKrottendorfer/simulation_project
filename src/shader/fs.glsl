#version 460

in vec4 colD; 
out vec4 color; 

//layout(location = 1) uniform sampler2D t;


void main() { 
    color = colD;
}