#include "stdafx.h"
#include "FinalRenderBuffer.h"

FinalRenderBuffer::FinalRenderBuffer()
{
}
FinalRenderBuffer::~FinalRenderBuffer()
{
}

bool FinalRenderBuffer::Init(int width, int height)
{
	m_iWrite = 0;

	m_width = width;
	m_height = height;

	glGenFramebuffers(1, &m_FBO);

	// Create the color buffer
	glGenTextures(2, m_color);
	glGenTextures(1, m_color);
	for (GLuint color : m_color)
	{
		glBindTexture(GL_TEXTURE_2D, color);
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color[m_iWrite], 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_NONE);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("FB error, status: 0x%x\n", Status);
			return false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void FinalRenderBuffer::BindForModification(GLenum textureUnit)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color[m_iWrite], 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_NONE);
	}

	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_color[!m_iWrite]);
}

void FinalRenderBuffer::SwapReadWriteTextures()
{
	m_iWrite = !m_iWrite;
}
