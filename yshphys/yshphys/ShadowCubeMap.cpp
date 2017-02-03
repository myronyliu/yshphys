#include "stdafx.h"
#include "ShadowCubeMap.h"

ShadowCubeMap::ShadowCubeMap() : m_near(1.0f), m_far(64.0f)
{
}

ShadowCubeMap::~ShadowCubeMap()
{
}

bool ShadowCubeMap::Init(int width)
{
	m_width = width;

	glGenFramebuffers(1, &m_FBO);

	// Create the depth buffer
	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, width, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

fMat44 ShadowCubeMap::CreateProjectionMatrix() const
{
	return fHomogeneousTransformation::CreateProjection(fPI*0.5f, 1.0f, m_near, m_far);
}

void ShadowCubeMap::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
}

void ShadowCubeMap::BindForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth);
}