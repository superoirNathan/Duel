#include "Texture.h"
#include <SOIL\SOIL.h>
#include <iostream>

Texture::Texture() {

}
Texture::~Texture() {
	unload();
}

bool Texture::load(const std::string &file) {
	texObj = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (texObj == NULL) 
	{
        std::cout << file << std::endl;
		std::cout << "Texture failed to load." << std::endl << SOIL_last_result() << std::endl;
		return false;
	}
	glBindTexture(GL_TEXTURE_2D, texObj);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //u axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //v axis
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return true;
}
void Texture::unload() {
	if (texObj != 0) 
	{
		glDeleteTextures(1, &texObj);
		texObj = 0;
	}
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, texObj);
}
void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

}