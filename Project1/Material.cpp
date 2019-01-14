#include "Material.h"
#include <iostream>

Material::Material() {

}
Material::Material(std::string diffusePath, std::string specularPath, float specExp, glm::vec3 &hue) 
	: specularExponent(specExp), hue(hue)
{

	loadTexture(TextureType::Diffuse, diffusePath);
	loadTexture(TextureType::Specular, specularPath);
}
Material::~Material() {

}

void Material::loadTexture(TextureType type, std::string texFile) {
	switch (type)
	{
	case Diffuse:
		if (!diffuse.load(texFile)) 
		{
			system("pause");
			exit(0);
		}
		break;
	case Specular:
		if (!specular.load(texFile)) 
		{
			system("pause");
			exit(0);
		}
		break;
	case Normal:
		if (!normal.load(texFile)) {
			system("pause");
			exit(0);
		}
		break;
    case BaseTexture:
        if (!texture.load(texFile)) {
            system("pause");
            exit(0);
        }
        break;
    case Warp:
       // if (!warp.load(texFile)) {
       //     system("pause");
       //     exit(0);
       // }
        break;
	default:
	{
        std::cout << texFile << std::endl;
		std::cout << "Error - TextureType not found: " << type << std::endl;
		system("pause");
		exit(0);
	}
		break;
	}

}

