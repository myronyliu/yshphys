#pragma once

class MouseMotionHandler
{
public:
	MouseMotionHandler();
	virtual ~MouseMotionHandler();

	void EnableMouseMotionProcessing();
	void DisableMouseMotionProcessing();
	
	bool MouseMotionProcessingEnabled() const;

	void ConditionalProcessMouseRelativeMotion(float xRel, float yRel);

protected:

	virtual void ProcessMouseRelativeMotion(int xRel, int yRel);

	float m_xSens;
	float m_ySens;

	bool m_mouseMotionProcessingEnabled;
};

