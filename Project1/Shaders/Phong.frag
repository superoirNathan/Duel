#version 420


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


uniform PointLight pointLight;
uniform Material material;
uniform int shaderNumber;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tang;
in vec3 biTang;


out vec4 outColor;

vec3 calculatePointLight(PointLight light, vec3 norm, vec4 materialDiffuse, vec4 materialSpecular);

layout (binding = 0) uniform sampler2D rampTexture;

vec3 regularLighting();
vec3 rampLighting();
vec3 rimLighting();

void main()
{
	// Throw everything into mat3 that will be sent to the shader
	mat3 tangentMatrix = mat3(tang.x, biTang.x, normal.x,
							tang.y, biTang.y, normal.y,
							tang.z, biTang.z, normal.z);
	
	
	//Diffuse
    vec4 baseColour = texture(material.texture, texCoord);
	vec3 L = normalize(vec3(pointLight.position.xyz) - vec3(0.f, -18.f, -1.3f));
	vec3 N = normalize(((texture(material.normal, texCoord).xyz) * 2.0 - 1) * tangentMatrix);	//Tangent Space
	//vec3 N = texture(material.normal, texCoord.xy).rgb * 2 - 1;														//Non - Tangent Space
	float ndotl = max(0.0, dot(N, L));
	float difIntensity = 0.8;
	vec3 difColour = vec3(0.5);
	
	if(shaderNumber == 1)
		difColour = vec3(0.5);
	if(shaderNumber == 2)
		difColour = vec3(0);
	if(shaderNumber == 3)
		difColour = vec3(texture(rampTexture, texCoord).rgb);
	if(shaderNumber == 4)
		difColour = vec3(texture(rampTexture, texCoord).rgb);	
	
	vec3 diffuse = difIntensity * vec3(ndotl) * difColour;		
	
	
	//ATTENUATION
	float dist = length(L);
	float attenuation = 1.0 / (pointLight.constantAttenuation + (pointLight.linearAttenuation * dist) + (pointLight.quadraticAttenuation * dist * dist));
	diffuse *= attenuation;
	diffuse = clamp(diffuse, 0, 1);
	
	// Old Attenuation values
		//float attenuation = 1.0 / (4.35 + (1.25 * dist) + (0.015 * dist * dist));
										
										
	//SPECULAR
	vec3 lightDir = L/dist;
	float NdotHV = max(dot(N, normalize(lightDir + normalize(-vec3(0.f, -18.f, -1.3f)))), 0.0);
	//vec3 specular =  pow(NdotHV, 32);
	NdotHV = clamp(NdotHV,0,1);
	
	vec3 specular = vec3(1.0) * pow(NdotHV, 32);
	
	if(shaderNumber == 1)
		specular = vec3(1.0) * pow(NdotHV, 32);
	if(shaderNumber == 2)
		specular = vec3(0);
	if(shaderNumber == 3)
		specular = texture(material.diffuse, vec2(pow(NdotHV, 32.f),0.0f), 0.0f).rgb;
	
	//specular = clamp(specular,0,1);
	//outColor.rgb = (normalize((texture(material.normal, texCoord).xyz) * 2.0 - 1).rgb * tangentToWorldMatrix) * 0.5 + 0.5;
	baseColour.xyz*=attenuation;
	specular*=attenuation;


	//outColor.rgb = texture(rampTexture, texCoord).rgb;
	if(shaderNumber == 0)
		outColor.rgb = regularLighting() + rimLighting();
	else if(shaderNumber == 1)
		outColor.rgb = baseColour.xyz + regularLighting();
	else if(shaderNumber == 2)
		outColor.rgb = baseColour.xyz;
	else if(shaderNumber == 3)
		outColor.rgb = baseColour.xyz + rampLighting();
	else if(shaderNumber == 4)
		outColor.rgb = baseColour.xyz + rampLighting();


}

vec3 regularLighting()
{
	// Throw everything into mat3 that will be sent to the shader
	mat3 tangentMatrix = mat3(tang.x, biTang.x, normal.x,
							tang.y, biTang.y, normal.y,
							tang.z, biTang.z, normal.z);

	//account for rasterizer interpolating
    vec4 baseColour = texture(material.texture, texCoord);
	vec3 L = normalize(vec3(pointLight.position.xyz) - vec3(0.f, -18.f, -1.3f));
	vec3 N = normalize(((texture(material.normal, texCoord).xyz) * 2.0 - 1) * tangentMatrix);	//Tangent Space

	//DIFFUSE
	float ndotl = max(0.0, dot(N, L));
	float difIntensity = 0.8;
	vec3 difColour = vec3(0.5);
	vec3 diffuse = difIntensity * ndotl * difColour;
	
	//ATTENUATION
	float dist = length(L);
	float attenuation = 1.0 / (pointLight.constantAttenuation + (pointLight.linearAttenuation * dist) + (pointLight.quadraticAttenuation * dist * dist));
	diffuse *= attenuation;
					
	//SPECULAR
	vec3 lightDir = L/dist;
	float NdotHV = max(dot(N, normalize(lightDir + normalize(-vec3(0.f, -18.f, -1.3f)))), 0.0);
	vec3 specular = vec3(1.0) * pow(NdotHV, 32);

	baseColour.xyz*=attenuation;
	specular*=attenuation;

	return (diffuse + specular);
}

vec3 rampLighting()
{
	// Throw everything into mat3 that will be sent to the shader
	mat3 tangentMatrix = mat3(tang.x, biTang.x, normal.x,
							tang.y, biTang.y, normal.y,
							tang.z, biTang.z, normal.z);

	//account for rasterizer interpolating
    vec4 baseColour = texture(material.texture, texCoord);
	vec3 L = normalize(vec3(pointLight.position.xyz) - vec3(0.f, -18.f, -1.3f));
	vec3 N = normalize(((texture(material.normal, texCoord).xyz) * 2.0 - 1) * tangentMatrix);	//Tangent Space

	//DIFFUSE
	float ndotl = max(0.0, dot(N, L));
	float difIntensity = 0.8;
	vec3 difColour = vec3(texture(rampTexture, texCoord).rgb);
	vec3 diffuse = difIntensity * ndotl * difColour;
	
	//ATTENUATION
	float dist = length(L);
	float attenuation = 1.0 / (pointLight.constantAttenuation + (pointLight.linearAttenuation * dist) + (pointLight.quadraticAttenuation * dist * dist));
	diffuse *= attenuation;
					
	//SPECULAR
	vec3 lightDir = L/dist;
	float NdotHV = max(dot(N, normalize(lightDir + normalize(-vec3(0.f, -18.f, -1.3f)))), 0.0);
	vec3 specular = texture(material.diffuse, vec2(pow(NdotHV, 32.f),0.0f), 0.0f).rgb;

	baseColour.xyz*=attenuation;
	specular*=attenuation;

	return (diffuse + specular);
}

vec3 rimLighting()
{
	mat3 tangentMatrix = mat3(tang.x, biTang.x, normal.x,
						tang.y, biTang.y, normal.y,
						tang.z, biTang.z, normal.z);
						
	vec3 v = normalize(-(vec3(0.f, -18.f, -1.3f)));
	vec3 n = normalize(normal);
	float stuff = 1.0 - max(dot(v, n), 0.0);

	return vec3(smoothstep(0.9f, 1.0, stuff));	
}