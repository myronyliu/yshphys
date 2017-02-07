#pragma once
#include "glew.h"
class FinalRenderBuffer
{
public:
	FinalRenderBuffer();
	virtual ~FinalRenderBuffer();

	virtual bool Init(int width, int height);

	GLuint GetLastWrittenColorTexture() const { return m_color[!m_iWrite]; }
	void BindForModification(GLenum textureUnit);

	void SwapReadWriteTextures();

	GLuint m_FBO;
	// We cannot modify pixels in place in the shader, so we maintain two textures, one for reading and one for writing
	GLuint m_color[2];

	int m_width;
	int m_height;

	int m_iWrite;
};

