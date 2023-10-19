#version 460

// in uvec3 gl_NumWorkGroups;
// in uvec3 gl_WorkGroupID;
// in uvec3 gl_LocalInvocationID;
// in uvec3 gl_GlobalInvocationID;
// in uint  gl_LocalInvocationIndex;

#define WORKGROUPS 1024

layout (local_size_x = WORKGROUPS, local_size_y = 1, local_size_z = 1) in;

layout (location = 0) uniform uint particle_count;
layout (location = 1) uniform uint crossing;

layout(std430, binding = 0) buffer layoutParticle{
    int pos[];
};

shared int local_pos[WORKGROUPS* 2];
shared uint swaps;

void local_swap(uint a, uint b) {
    if(local_pos[a] > local_pos[b]) {

        atomicCompSwap(swaps,0,1);

        int temp = local_pos[a];
        local_pos[a] = local_pos[b];
        local_pos[b] = temp;
    }
}

void main() {

	const uint t = gl_LocalInvocationID.x * 2;
    if(gl_LocalInvocationID.x == 0){
        atomicExchange(swaps,1);
    }

	uint offset =  WORKGROUPS * 2 * gl_WorkGroupID.x; 

    if(crossing > 0){
        offset +=  WORKGROUPS;
    }

    local_pos[t]   = pos[offset+t];
    local_pos[t+1] = pos[offset+t+1];

    barrier();
    uint tt;
    while(swaps > 0){
        if(gl_LocalInvocationID.x == 0){
            atomicExchange(swaps,0);
        }
        barrier();

        tt = t;
        if(tt + 1 < WORKGROUPS * 2){
            local_swap(tt,tt+1);
        }
        barrier();

        tt = t + 1;
        if(tt + 1 < WORKGROUPS * 2){
            local_swap(tt,tt+1);
        }
        barrier();
    }

    pos[offset+t] = local_pos[t];    
    pos[offset+t+1] = local_pos[t+1];

};
