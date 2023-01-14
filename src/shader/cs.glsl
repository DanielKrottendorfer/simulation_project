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
layout(std430, binding = 9) buffer fixedIn {
    uint stiff[];
};

void main() {
    if(stiff[gl_GlobalInvocationID.x] == 0 && moveVertex == 1)
    {
        vertIn[gl_GlobalInvocationID.x].x -= 0.1;
    }
}