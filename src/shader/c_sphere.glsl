#version 460 core

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(location = 0) uniform vec3 sphere_pos;
layout(location = 1) uniform float sphere_rad;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

float MAGIC_S = 0.5; //MAGIC;

void main() {

    vec3 v = vertIn[gl_GlobalInvocationID.x].xyz;

    vec3 v_to_sphere = v - sphere_pos;
    float len = length(v_to_sphere);

    if(len < sphere_rad){

        vec3 v_to_sphere_edge = v_to_sphere / len;
        v_to_sphere_edge *= sphere_rad;

        vec3 cor = v_to_sphere_edge - v_to_sphere;

        vertIn[gl_GlobalInvocationID.x].xyz += cor;
    }

}