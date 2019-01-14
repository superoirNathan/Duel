#version 420

// Remember to bind the two textures to different texture units!
layout(binding = 10) uniform sampler2D u_bright; // bright pass image
layout(binding = 11) uniform sampler2D u_scene;	// original scene image

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

void main()
{
	// COMPOSITE BLOOM HERE
	// - Sample from the two textures and add the colors together

	vec4 scene = texture(u_scene, texCoord.xy) + texture(u_bright, texCoord.xy);

	outColor = scene;
}