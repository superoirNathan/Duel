#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

layout(location=0)in vec3 in_vert;
layout(location=1) in vec2 in_uv;
layout(location=2) in vec3 in_normal;

out vec3 normals;

out VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec3 posEye;
} vOut;

void main(){

    vOut.normal = mat3(uView) * mat3(uModel) * in_normal;
    vec4 viewSpacePos = uView * uModel * vec4(in_vert, 1.0f);
	vOut.posEye = viewSpacePos.xyz;
	vOut.texCoord = vec3(in_uv,0);
	                                    
	normals = in_normal;
	gl_Position = uProj * uView * uModel * vec4(in_vert, 1.0f);

}
