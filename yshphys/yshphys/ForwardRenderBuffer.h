#pragma once
#include "YshMath.h"
#include "glew.h"
class ForwardRenderBuffer
{
public:
	ForwardRenderBuffer();
	virtual ~ForwardRenderBuffer();

	virtual bool Init(int width, int height);

	void BindForWriting();

	void BindDiffuseForReading(GLenum textureUnit);
	void BindSpecularForReading(GLenum textureUnit);
	void BindNormalForReading(GLenum textureUnit);
	void BindPositionForReading(GLenum textureUnit);
	void BindStencilForReading(GLenum textureUnit);

	GLuint m_FBO;

	GLuint m_position;
	GLuint m_normal;

	GLuint m_diffuse;
	GLuint m_specular;

	GLuint m_stencil; // for determining whether parts of the image should be included in the lighting and shadow computations

	int m_width;
	int m_height;

protected:

	GLuint m_depthBuffer; // internal buffer for depth testing
};

