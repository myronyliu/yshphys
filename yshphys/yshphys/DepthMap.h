#pragma once
#include "Viewport.h"
#include "glew.h"
class DepthMap
{
public:
	DepthMap();
	virtual ~DepthMap();

	virtual bool Init(int width, int height);

//	virtual fMat44 CreateProjectionMatrix() const;

	void BindForWriting();

	void BindForReading(GLenum TextureUnit);

	GLuint m_FBO;
	GLuint m_depth;

	int m_width;
	int m_height;

	Viewport m_viewport;
};

