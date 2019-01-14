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

	outColor.r = (texture(material.diffuse, vec2(scene.r, 0.0)).r);	
	outColor.g = (texture(material.diffuse, vec2(scene.g, 0.0)).g);
	outColor.b = (texture(material.diffuse, vec2(scene.b, 0.0)).b);

	float avg = outColor.r + outColor.g + outColor.b;
	float avg2 = scene.r + scene.g + scene.b;


	vec3 colorAvg = vec3((avg)/3.0);
	vec3 colorAvg2 = vec3((avg2)/3.0);

	outColor.rgb =  (colorAvg * colorAvg2);

	outColor.a = 1;
}