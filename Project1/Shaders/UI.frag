#version 420

#define NUM_POINT_LIGHTS 2

struct PointLight
{
	vec4 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct DirectionalLight
{
	vec4 direction;

	float specularExponent;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;

	vec3 hue;

	float specularExponent;
};

uniform Material material;

in vec2 texCoord;
out vec4 outColor;

void main()
{

	vec4 diffuse = texture(material.diffuse, texCoord);

	outColor = diffuse;
}
