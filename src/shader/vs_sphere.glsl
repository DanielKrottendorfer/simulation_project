#version 460

layout(location = 0) in vec3 pos; 
layout(location = 1) in vec3 norm; 

layout(location = 0) uniform mat4 MVP;

out VS_OUT {
    vec3 norm;
    vec3 pos;
} vs_out;

void main() { 
    vs_out.norm = norm;
    vs_out.pos = pos;
    
    gl_Position = MVP * vec4(pos,1.0);
}
