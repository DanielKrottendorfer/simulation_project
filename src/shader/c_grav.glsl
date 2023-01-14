#version 460 core

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

layout(std430, binding = 9) buffer fixedIn {
    uint stiff[];
};

layout(std430, binding = 10) buffer vertexP {
    vec4 vertInP[];
};

layout(std430, binding = 11) buffer velocityIn {
    vec4 vel[];
};

void main() {

    if (stiff[gl_GlobalInvocationID.x] == 0) {
        vel[gl_GlobalInvocationID.x].y -= 0.001;
        vertInP[gl_GlobalInvocationID.x] = vertIn[gl_GlobalInvocationID.x];
        vertIn[gl_GlobalInvocationID.x] += vel[gl_GlobalInvocationID.x];
    }
}