#pragma once

#include <GL\glew.h>
#include <string>

enum TextureType
{
	Diffuse,
	Specular,
	Normal
};

class Texture
{
public:
	Texture();
	~Texture();

	bool load(const std::string &file);
	void unload();

	void bind();
	void unbind();

	// The handle to the texture object
	GLuint texObj = 0;
};