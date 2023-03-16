#version 450 core

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define WORKGROUPS 1024

layout (local_size_x = WORKGROUPS, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer SortData 
{
	uint value[];
};

layout (location = 0) uniform uint p_count;


// Performs compare-and-swap over elements held in shared,
// workgroup-local memory
#define COMPARE is_smaller

bool is_smaller(in const uint left, in const uint right){
	return left < right;
}

void local_compare_and_swap(ivec2 idx){
	if (COMPARE(value[idx.x] ,value[idx.y])) {
		uint tmp = value[idx.x];
		value[idx.x] = value[idx.y];
		value[idx.y] = tmp;
	}
}


// Performs full-height flip (h height) over locally available indices.
void local_flip(in uint h, uint c){
	uint offs = 2 << c;
	uint t = gl_LocalInvocationID.x * offs;
	barrier();

	for( uint i=0 ; i<=offs; ++i){

		uint tt = t + i;
		uint half_h = h >> 1; // Note: h >> 1 is equivalent to h / 2 
		ivec2 indices = 
			ivec2( h * ( ( 2 * tt ) / h ) ) +
			ivec2( tt % half_h, h - 1 - ( tt % half_h ) );

		local_compare_and_swap(indices);
	}
}

// Performs progressively diminishing disperse operations (starting with height h)
// on locally available indices: e.g. h==8 -> 8 : 4 : 2.
// One disperse operation for every time we can divide h by 2.
void local_disperse(in uint h, uint c){
	uint offs = 2 << c;
	uint t = gl_LocalInvocationID.x * offs;
	for ( ; h > 1 ; h /= 2 ) {
		
		barrier();

		for( uint i=0 ; i<=offs; ++i){

			uint tt = t + i;

			uint half_h = h >> 1; // Note: h >> 1 is equivalent to h / 2 
			ivec2 indices = 
				ivec2( h * ( ( 2 * tt ) / h ) ) +
				ivec2( tt % half_h, half_h + ( tt % half_h ) );

			local_compare_and_swap(indices);
		}

	}
}

// Perform binary merge sort for local elements, up to a maximum number 
// of elements h.
void local_bms(uint h, uint c){
	uint t = gl_LocalInvocationID.x;
	for ( uint hh = 2; hh <= h; hh <<= 1 ) {  // note:  h <<= 1 is same as h *= 2
		local_flip( hh,c);
		local_disperse( hh/2 ,c);
	}
}

void main(){
	
	uint t = gl_LocalInvocationID.x;


 
	local_bms(p_count, (p_count+1) / WORKGROUPS);

}