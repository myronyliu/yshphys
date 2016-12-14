#include "stdafx.h"
#include "MouseMotionHandler.h"


MouseMotionHandler::MouseMotionHandler() : m_xSens(0.001f), m_ySens(0.001f), m_mouseMotionProcessingEnabled(true)
{
}


MouseMotionHandler::~MouseMotionHandler()
{
}

void MouseMotionHandler::EnableMouseMotionProcessing()
{
	m_mouseMotionProcessingEnabled = true;
}
void MouseMotionHandler::DisableMouseMotionProcessing()
{
	m_mouseMotionProcessingEnabled = false;
}

bool MouseMotionHandler::MouseMotionProcessingEnabled() const
{
	return m_mouseMotionProcessingEnabled;
}

void MouseMotionHandler::ConditionalProcessMouseRelativeMotion(float xRel, float yRel)
{
	if (m_mouseMotionProcessingEnabled)
	{
		ProcessMouseRelativeMotion(xRel, yRel);
	}
}

void MouseMotionHandler::ProcessMouseRelativeMotion(float xRel, float yRel)
{
}

