#version 450

in GS_OUT {
    vec3 norm;
    vec3 pos;
} fs_in;

out vec4 out_color;

vec3 l = vec3(-1,-1,-1);

layout(location = 1) uniform vec3 eye_pos;

void main(){

    vec3 col = vec3(1,0,1);
    vec3 light = normalize(l);

    float diff_l = dot(light,fs_in.norm);

    diff_l = diff_l*diff_l;


    vec3 pos_to_eye = normalize(eye_pos - fs_in.pos);

    float spec_l = dot(pos_to_eye,light);
    spec_l *= spec_l;
    float spec_2 = pow(spec_l, 20.0);

	out_color = vec4((col)*(diff_l+spec_2),1.0);
}