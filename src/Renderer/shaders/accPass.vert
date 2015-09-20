#version 450 core

layout(location = 0) in vec3 in_pos;

out vec2 UV;

void main(){
	gl_Position =  vec4(in_pos,1.0);
	UV = in_pos.xy*0.5 + 0.5;
}

