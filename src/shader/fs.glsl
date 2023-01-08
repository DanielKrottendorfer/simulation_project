#version 460

//in vec2 uv; 
out vec4 color; 

//layout(location = 1) uniform sampler2D t;

layout(std430, binding = 4) buffer colorIn{
    vec4 colIn;
};

void main() { 
    color = colIn;
}