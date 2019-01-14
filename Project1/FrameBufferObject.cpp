#include "FrameBufferObject.h"
#include <iostream>
 
FrameBufferObject::FrameBufferObject()
	: depthTexHandle(0)
{
	memset(bufferAttachments, 0, sizeof(GLenum) * 16);
	memset(colourTexHandles, 0, sizeof(unsigned int) * 16);

}

FrameBufferObject::~FrameBufferObject()
{
	destroy();
}

// Render buffers do not have colour attachments and are used to store data
// for OpenGL logic operations such as depth testing.
// Frame Buffers do not store any actual data but instead we attach textures to them.
// We can write to the textures in a fragment shader.

void FrameBufferObject::createFrameBuffer(unsigned int fboWidth, unsigned int fboHeight, unsigned int numColourBuffers, bool useDepth)
{
	width = fboWidth;
	height = fboHeight;
	numColorTex = numColourBuffers;

	// Creating a handle for a FBO
	// This is an empty FBO
	glGenFramebuffers(1, &handle);

	// Bind the FBO
	// Tell OpenGL we want to do things to this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	// We can have multiple textures on one frame buffer object
	// So we need to generate an id for each of them
	glGenTextures(numColorTex, colourTexHandles); // CHANGED

	// We need to initialize all of the textures
	for (int i = 0; i < numColorTex; i++)
	{
		// Bind the texture
		// binding tells OpenGL we want to do something with this texture
		glBindTexture(GL_TEXTURE_2D, colourTexHandles[i]);

		// We need to initialize the size of the texture
		// Here I am making each texture the same size, but you may want to
		// extend this class to allow textures of different size
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// Texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Now that we have created handles and allocated memory for the texture
		// We need to add them to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourTexHandles[i], 0);

		// Need to keep track of the attachments added 
		bufferAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	// An FBO can have a single depth map
	if (useDepth)
	{
		glGenTextures(1, &depthTexHandle);
		glBindTexture(GL_TEXTURE_2D, depthTexHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexHandle, 0);
	}

	// Set up which layout location in the fragment shader goes to which
	// colour attachment. This is a part of the fbo's state so we do not
	// have to call it every frame
	glDrawBuffers(numColorTex, bufferAttachments);

	// We just did a lot of stuff
	// It's a good idea to check if we did everything correctly
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error Creating FBO " << fboStatus << std::endl;
	}

	// Unbind FBO
	// When we unbind an FBO it goes back to the system provided FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::bindFrameBufferForDrawing()
{
	glBindFramebuffer(GL_FRAMEBUFFER, handle);
	glViewport(0, 0, width, height);
}

void FrameBufferObject::bindDepthTextureForSampling(GLenum textureUnit)
{
	if (depthTexHandle)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, depthTexHandle);
	}
	else
		std::cout << "FBO does not have a depth texture!" << std::endl;
}

void FrameBufferObject::unbindFrameBuffer(int backBufferWidth, int backBufferHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, backBufferWidth, backBufferHeight);
}

void FrameBufferObject::clearFrameBuffer(glm::vec4 clearColour)
{
	glClearColor(clearColour.x, clearColour.y, clearColour.z, clearColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void FrameBufferObject::bindTextureForSampling(int textureAttachment, GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, colourTexHandles[textureAttachment]);
}

void FrameBufferObject::unbindTexture(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::destroy()
{
	if (colourTexHandles[0])
		glDeleteTextures(numColorTex, colourTexHandles);

	if (depthTexHandle)
	{
		glDeleteTextures(1, &depthTexHandle);
		depthTexHandle = 0;
	}

	if (handle)
		glDeleteFramebuffers(1, &handle);

	unbindFrameBuffer(width, height);
}


