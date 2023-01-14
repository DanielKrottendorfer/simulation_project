#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) uniform mat4 proj;

out GS_OUT {
    vec3 norm;
    vec3 pos;
} gs_out;

void main()
{
    vec3 u =  gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 v =  gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 n = vec3(0,0,0);
    n.x = (u.y * v.z) - (u.z * v.y);
    n.y = (u.z * v.x) - (u.x * v.z);
    n.z = (u.x * v.y) - (u.y * v.x);
    n = normalize(n);

    gl_Position =  proj * gl_in[0].gl_Position;
    //gs_out.color = vs_out[0].color;
    gs_out.norm = n;
    gs_out.pos = gl_in[0].gl_Position.xyz;
    EmitVertex();

    gl_Position =  proj * gl_in[1].gl_Position;
    gs_out.pos = gl_in[1].gl_Position.xyz;
    //gs_out.color = vs_out[1].color;
    gs_out.norm = n;
    EmitVertex();
    
    gl_Position =  proj * gl_in[2].gl_Position;
    gs_out.pos = gl_in[2].gl_Position.xyz;
    //gs_out.color = vs_out[2].color;
    gs_out.norm = n;
    EmitVertex();

    EndPrimitive();

}  

