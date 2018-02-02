#version  420

in vec3 normals;

out vec4 outColor;

void main(){
	outColor = vec4(normals,1.0f);


}