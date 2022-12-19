#version 460

layout(location = 0) in vec2 Pos2D; 
layout(location = 1) in vec2 UV2D; 

layout(location = 0) uniform mat4 proj;

out vec2 uv; 

void main() { 
    uv = UV2D;
    gl_Position = proj * vec4( Pos2D, 0, 1 ); 
}
