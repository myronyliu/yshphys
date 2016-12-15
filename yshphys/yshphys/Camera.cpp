#include "stdafx.h"
#include "Camera.h"
#include "YshMath.h"

#define MIN_CAMERA_THETA (fPI * 0.01f)
#define MAX_CAMERA_THETA (fPI * 0.99f) 

Camera::Camera() : m_theta(fPI * 0.5f), m_phi(fPI * 0.5f), m_pos(0.0f, 0.0f, 0.0f), m_moveSpeed(16.0f, 16.0f, 16.0f)
{
	m_mappedKeys[MOVE_FORWARD] = YSH_INPUT_W;
	m_mappedKeys[MOVE_BACKWARD] = YSH_INPUT_S;
	m_mappedKeys[MOVE_RIGHTWARD] = YSH_INPUT_D;
	m_mappedKeys[MOVE_LEFTWARD] = YSH_INPUT_A;
	m_mappedKeys[MOVE_UPWARD] = YSH_INPUT_SPACE;
	m_mappedKeys[MOVE_DOWNWARD] = YSH_INPUT_LSHIFT;
}


Camera::~Camera()
{
}

void Camera::PanUp(float pitch)
{
	m_theta -= pitch;
	if (m_theta < MIN_CAMERA_THETA)
	{
		m_theta = MIN_CAMERA_THETA;
	}
	else if (m_theta > MAX_CAMERA_THETA)
	{
		m_theta = MAX_CAMERA_THETA;
	}
}

void Camera::PanRight(float yaw)
{
	m_phi -= yaw;
	m_phi -= floor(m_phi / (2.0f*fPI)) * 2.0f*fPI;
}

void Camera::UpdateView()
{
	const fVec3 v(sin(m_theta)*cos(m_phi), sin(m_theta)*sin(m_phi), cos(m_theta));
	m_viewport->SetViewDir(v, fVec3(0.0f, 0.0f, 1.0f));
	m_viewport->SetPos(m_pos);
}

void Camera::SetViewport(Viewport* viewport)
{
	m_viewport = viewport;
}

unsigned int Camera::GetNumMappedKeys() const
{
	return Camera::KeyActions::N_KEY_ACTIONS;
}

void Camera::ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt)
{
	PanRight((float)mouseState.m_xRel * m_xSens);
	PanUp(-(float)mouseState.m_yRel * m_ySens);

	fVec3 dPos;

	dPos.y =
		float(keyStates[MOVE_FORWARD].m_state == KeyState::State::PRESSED) -
		float(keyStates[MOVE_BACKWARD].m_state == KeyState::State::PRESSED);
	dPos.x =
		float(keyStates[MOVE_RIGHTWARD].m_state == KeyState::State::PRESSED) -
		float(keyStates[MOVE_LEFTWARD].m_state == KeyState::State::PRESSED);
	dPos.z =
		float(keyStates[MOVE_UPWARD].m_state == KeyState::State::PRESSED) -
		float(keyStates[MOVE_DOWNWARD].m_state == KeyState::State::PRESSED);

	dPos = dPos.Times(m_moveSpeed.Scale((float)dt*0.001f));
	
	fMat33 basis; // not orthonormal
	const fVec3 y(sin(m_theta)*cos(m_phi), sin(m_theta)*sin(m_phi), cos(m_theta));
	const fVec3 x(cos(m_phi - fPI*0.5f), sin(m_phi - fPI*0.5f), 0.0f);
	basis.SetColumn(0, x);
	basis.SetColumn(1, y);
	basis.SetColumn(2, fVec3(0.0f, 0.0f, 1.0f));

	m_pos = m_pos + basis.Transform(dPos);
	UpdateView();
}