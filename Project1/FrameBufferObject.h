#pragma once

#include "GL\glew.h"
#include "glm/glm.hpp"

class FrameBufferObject
{
public:
	FrameBufferObject();
	~FrameBufferObject();

	void createFrameBuffer(unsigned int fboWidth, unsigned int fboHeight, unsigned int numColourBuffers, bool useDepth);

	// Set active frame buffer for rendering
	void bindFrameBufferForDrawing();
	void bindDepthTextureForSampling(GLenum textureUnit);
	static void unbindFrameBuffer(int backBufferWidth, int backBufferHeight);

	static void clearFrameBuffer(glm::vec4 clearColour);

	// Bind specific textures
	// Allows us to sample textures in a shader
	void bindTextureForSampling(int textureAttachment, GLenum textureUnit);
	void unbindTexture(GLenum textureUnit);

	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }

	// Frees GPU memory allocated by this FBO
	void destroy();

private:
	unsigned int numColorTex;  // NEW: We need to keep track of the number of textures on the FBO

	// Handle for FBO itself
	unsigned int handle;

	// Handle for texture attachments
	// Multiple colour textures can be attached to a single FBO
	// Fragment shader can output multiple values
	unsigned int colourTexHandles[16]; // CHANGES: This is now an array to hold each ID

	// Handle for depth texture attachment
	// Can only have one depth texture
	// Depth is calculated based on vertex positions
	// Before the fragment shader
	unsigned int depthTexHandle;

	// Dimensions of textures
	unsigned int width, height;

	// Attachments
	GLenum bufferAttachments[16]; // CHANGED: this is now an array, since we can have multiple attachments
};