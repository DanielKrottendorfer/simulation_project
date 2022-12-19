#version 460

in vec2 uv; 
out vec4 color; 

layout(location = 1) uniform sampler2D t;

void main() { 
    color = texture(t , uv).rgba;
}
