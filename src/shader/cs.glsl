#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(location = 7) uniform int moveVertex;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

layout(std430, binding = 4) buffer colorIn{
    vec4 colIn[];
};

layout(std430, binding = 5) buffer edgeIn{
    uint[2] edIn[];
};


	
void main() {
    if(moveVertex == 1 && gl_GlobalInvocationID.x == 0)
    {
        vertIn[0].x = -80.8;
    }
    if(gl_GlobalInvocationID.x % 3 == 0)
    {
        vertIn[gl_GlobalInvocationID.x].x = -80.8;
        colIn[gl_GlobalInvocationID.x] = vec4(0, 1, 1, 1);
    }
}