#pragma once

#include <GL\glew.h>
#include <string>

enum TextureType
{
    // These are maps, they alter the light intensity(diffuse/specular maps) and calculations(normal maps)
    Diffuse,
	Specular,
	Normal,
    // These are textures that are the intended base colour of the object
    BaseTexture,
    // These are colour correction textures(Warp/Ramp)
    Warp
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