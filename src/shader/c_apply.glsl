#version 460 core

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(location = 7) uniform int moveVertex;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

layout(std430, binding = 4) buffer colorIn {
    vec4 colIn[];
};

layout(std430, binding = 5) buffer edgeIn {
    uint[2] edIn[];
};

layout(std430, binding = 6) buffer vertexOut {
    vec4 cor[];
};  

layout(std430, binding = 7) buffer refIn {
    uint[4] refs[];
};
layout(std430, binding = 8) buffer lenIn {
    uint lens[];
};

layout(std430, binding = 9) buffer fixedIn {
    uint stiff[];
};

void main() {

    uint len = lens[gl_GlobalInvocationID.x];
    for (uint i = 0; i < len; ++i) {
        uint j = refs[gl_GlobalInvocationID.x][i];

        if (edIn[j][0] == gl_GlobalInvocationID.x) {
            if (stiff[edIn[j][0]] == 0) {
                vertIn[gl_GlobalInvocationID.x] += cor[j];
            }
        }
        else {
            if (stiff[edIn[j][1]] == 0) {
                vertIn[gl_GlobalInvocationID.x] -= cor[j];
            }
        };
    };


}