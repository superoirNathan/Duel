#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

uniform float in_width;
uniform float in_height;
uniform float in_time;

layout(location = 0) in vec3 in_vert;

void main(){
	gl_Position = vec4(in_vert, 1.0f);
}