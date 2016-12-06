#pragma once
class MouseMotionHandler
{
public:
	MouseMotionHandler();
	virtual ~MouseMotionHandler();

	virtual void ProcessMouseRelativeMotion(float xRel, float yRel);
protected:
	float m_xSens;
	float m_ySens;
};

