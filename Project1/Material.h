#pragma once
#include  <glm\glm.hpp>
#include "Texture.h"

class Material {
public:
	Material();
	Material(std::string diffusePath, std::string specularPath, float specExp = 50.f, glm::vec3 &hue = glm::vec3(1.f, 1.f, 1.f));
	~Material();


	void loadTexture(TextureType type, std::string texFile);

	//Data Memebers
	Texture diffuse, specular, normal;

	glm::vec3 hue = glm::vec3(1.f, 1.f, 1.f);
	float specularExponent = 50.f;

};