#version 420

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


	outColor = texture(material.texture, texCoord);
//	outColor.rg = texCoord;

}