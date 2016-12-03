#pragma once
#include "Viewport.h"
class Camera
{
public:
	Camera();
	virtual ~Camera();
private:
	Viewport* m_viewport;
};

