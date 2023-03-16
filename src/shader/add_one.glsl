#version 460 core

// in uvec3 gl_NumWorkGroups;
// in uvec3 gl_WorkGroupID;
// in uvec3 gl_LocalInvocationID;
// in uvec3 gl_GlobalInvocationID;
// in uint  gl_LocalInvocationIndex;

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer layoutParticle{
    int v[];
};

void main() {

    v[gl_GlobalInvocationID.x] += 1;

};
