#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform float Time;


layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_vert2;
layout(location = 4) in vec3 in_normal2;

out vec3 position;
out vec2 texCoord;
out vec3 normal;

void main(){
	vec3 outVert = mix(in_vert,in_vert2,Time);
	vec3 outNormal = mix(in_normal,in_normal2,Time);

	texCoord = in_uv;
	normal = mat3(uView) * mat3(uModel) * outNormal;

	vec4 viewSpacePos = uView * uModel * vec4(outVert, 1.0f);
	position = viewSpacePos.xyz;
	gl_Position = uProj * viewSpacePos;
}