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

	GLuint m_FBO;

	GLuint m_diffuse;
	GLuint m_specular;
	GLuint m_position;
	GLuint m_normal;

	int m_width;
	int m_height;
	float m_near;
	float m_far;

protected:

	GLuint m_depthBuffer; // internal buffer for depth testing
};

