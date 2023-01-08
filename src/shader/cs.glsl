#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 3) buffer vertexIn{
    vec4 vertIn[];
};

layout(std430, binding = 4) buffer colorIn{
    vec4 colIn[];
};


	
void main() {
	colIn[gl_GlobalInvocationID.x] = vec4(0, 1, 1, 1);
}