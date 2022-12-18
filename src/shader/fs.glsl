#version 460

in vec2 p; 
out vec4 color; 

layout(location = 0) uniform sampler2D t;

void main() { 
    vec2 temp = vec2(0.5,0.5);
    color = texture(t , p + temp).rgba;
}
