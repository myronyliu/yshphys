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

	void BindForWriting(GLenum CubeFace);

	void BindForReading(GLenum TextureUnit);


	GLuint m_FBO;
	GLuint m_shadowMap;
	GLuint m_depth;
};

