#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform float in_uvScroll;

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tang;
layout(location = 4) in vec3 in_biTang;

out vec3 position;
out vec2 texCoord;
out vec3 normal;
out vec3 tang;
out vec3 biTang;

void main(){
	texCoord = in_uv;
//	texCoord.x = mix( max(0, texCoord.x - 0.5), min( (texCoord.x + 0.5), 1) ,in_uvScroll);
	texCoord.x += in_uvScroll;
//	texCoord.x = in_uvScroll;

	normal = mat3(uView) * mat3(uModel) * in_normal;
	tang = mat3(uView) * mat3(uModel) * in_tang;
	biTang = mat3(uView) * mat3(uModel) * in_biTang;

	vec4 viewSpacePos = uView * uModel * vec4(in_vert, 1.0f);
	
    position = viewSpacePos.xyz;
	gl_Position = uProj * viewSpacePos;
}   