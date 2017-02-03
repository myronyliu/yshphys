#pragma once
#include "YshMath.h"
#include "glew.h"

// Ripped from http://ogldev.atspace.co.uk/www/tutorial43/tutorial43.html

class ShadowCubeMap
{
public:
	ShadowCubeMap();
	virtual ~ShadowCubeMap();

	virtual bool Init(int width);

	virtual fMat44 CreateProjectionMatrix() const;

	void BindForWriting();

	void BindForReading(GLenum TextureUnit);

	GLuint m_FBO;
	GLuint m_depth;

	float m_near;
	float m_far;

	int m_width;
};

