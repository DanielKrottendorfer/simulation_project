#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

float MAGIC_S = 0.5; //MAGIC;

void main() {

    uint i1 = edIn[gl_GlobalInvocationID.x][0];
    uint i2 = edIn[gl_GlobalInvocationID.x][1];

    vec4 x1 = vertIn[i1];
    vec4 x2 = vertIn[i2];

    vec3 g = x1.xyz - x2.xyz;
    float len = length(g);
    vec4 c = vec4(g / len, 0);

    float lamb = -(len - 0.01) / (2);

    cor[gl_GlobalInvocationID.x] = c * lamb * MAGIC_S;
}