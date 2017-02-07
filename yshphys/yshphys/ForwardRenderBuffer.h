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
//	void BindForReading(GLenum textureUnit);
	void BindColorForReading(GLenum textureUnit);
	void BindNormalForReading(GLenum textureUnit);
	void BindDepthForReading(GLenum textureUnit);

	GLuint m_FBO;

	GLuint m_color; // the texture into which the colored image is rendered
	GLuint m_depth;
	GLuint m_normal;

	int m_width;
	int m_height;
	float m_near;
	float m_far;

protected:

	GLuint m_depthBuffer; // internal buffer for depth testing
};

