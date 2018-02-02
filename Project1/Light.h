#pragma once
#include <glm\glm.hpp>

class Light {//storage class for everything to be sent to shader
public:
	Light();
	~Light();
	
	glm::vec4 originalPosition;
	glm::vec4 positionOrDirection;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float specularExponent;
	
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

};