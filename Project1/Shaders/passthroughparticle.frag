#version  420

// Note: Uniform bindings
// This lets you specify the texture unit directly in the shader!
layout(binding = 10) uniform sampler2D u_rgb; // rgb texture

struct Material
{
    //Maps
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
    
    //Texture
    sampler2D texture;
    
    //WARP_1
    sampler2D warp;

    //Variables
	vec3 hue;
	float specularExponent;
};

uniform Material material;


in vec3 position;
in vec2 texCoord;

out vec4 outColor;


void main(){
	outColor = texture(u_rgb, texCoord);

}