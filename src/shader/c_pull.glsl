#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

layout(std430, binding = 9) buffer fixedIn {
    uint stiff[];
};

void main() {
    if(stiff[gl_GlobalInvocationID.x] == 0)
    {
        vertIn[gl_GlobalInvocationID.x].x -= 0.1;
    }
}