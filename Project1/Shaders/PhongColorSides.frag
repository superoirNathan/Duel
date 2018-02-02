#version 420

#define NUM_POINT_LIGHTS 2
#define UP_COLOR vec3(0, 0, 1)
#define DOWN_COLOR vec3(1, 0, 0)
#define LEFT_COLOR vec3(1, 0, 1)
#define RIGHT_COLOR vec3(0, 1, 0)
#define FRONT_COLOR vec3(0, 1, 1)
#define BACK_COLOR vec3 (1, 1, 0)

struct PointLight
{
	vec4 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float specularExponent;

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

uniform PointLight pointLights[NUM_POINT_LIGHTS];

uniform DirectionalLight directionalLight;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 modelNormal;

out vec4 outColor;

vec3 calculatePointLight(PointLight light, vec3 norm);

vec3 calculateDirectionalLight(DirectionalLight light, vec3 norm);

void main()
{
	//account for rasterizer interpolating
	vec3 norm = normalize(normal);

	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		outColor.rgb += calculatePointLight(pointLights[i], norm);	
	}

	vec3 color = max(modelNormal.x, 0.0) * RIGHT_COLOR + max(-modelNormal.x, 0.0) * LEFT_COLOR + 
				max(modelNormal.y, 0.0) * UP_COLOR + max(-modelNormal.y, 0.0) * DOWN_COLOR + 
				max(modelNormal.z, 0.0) * FRONT_COLOR + max(-modelNormal.z, 0.0) * BACK_COLOR;

	outColor.rgb *= color;
	outColor.a = 1.0;
}

vec3 calculatePointLight(PointLight light, vec3 norm)
{
	vec3 lightVec = light.position.xyz - position;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;


	
	//The light contributes to this surface

	//Calculate the attenuation (falloff)
	float attenuation = 1.0 / (light.constantAttenuation + (light.linearAttenuation * dist) + (light.quadraticAttenuation * dist * dist));
	
	vec3 ambient = attenuation * light.ambient;	
		
	//Calc the DIFFUSE contribution
	float NdotL = max(dot(normal, lightDir),0.0);
	vec3 diffuse = light.diffuse * NdotL * attenuation;
	
	//SPECULAR
	vec3 reflectDir = reflect(-lightDir, norm);
	float VdotR = max(dot(normalize(-position),reflectDir),0.0);
	vec3 specular =light.specular * pow(VdotR, light.specularExponent) * attenuation* light.specular;
	
	return ambient + diffuse + specular;
}

vec3 calculateDirectionalLight(DirectionalLight light, vec3 norm, vec4 materialDiffuse, vec4 materialSpecular){
	vec3 lightDir = normalize(-light.direction).xyz;

	vec3 ambient = light.ambient;	
		
	// Diffuse
	float NdotL = max(dot(normal, lightDir),0.0);
	vec3 diffuse = light.diffuse * NdotL * materialDiffuse.rgb;
	
	//SPECULAR
	vec3 reflectDir = reflect(-lightDir, norm);
	float VdotR = max(dot(normalize(-position),reflectDir),0.0);
	vec3 specular = light.specular * pow(VdotR, light.specularExponent) * light.specular * materialSpecular.rgb;
	
	return ambient + diffuse + specular;
}