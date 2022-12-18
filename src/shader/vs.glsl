#version 460

layout(location = 0) in vec2 LVertexPos2D; 

out vec2 p; 

void main() { 
    p = LVertexPos2D;
    gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); 
}
