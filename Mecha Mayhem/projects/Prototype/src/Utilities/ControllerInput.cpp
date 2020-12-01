#include "ControllerInput.h"

ControllerInput::Controller ControllerInput::m_controllers[XUSER_MAX_COUNT] = {};

void ControllerInput::ControllerRefresh()
{
	//This basically checks all the controllers, and if they are plugged in
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			m_controllers[i].connected = true;
			m_controllers[i].gamepad = state.Gamepad;
		}
		else
		{
			// Controller is not connected
			m_controllers[i].connected = false;
		}
	}
}

void ControllerInput::ControllerUpdate()
{
	//This basically checks all the controllers, and if they are plugged in
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Simply get the state of the controller from XInput.
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			m_controllers[i].connected = true;

			//updating for the getdown functions
			m_controllers[i].faceButtons[0] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_A;
			m_controllers[i].faceButtons[1] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_B;
			m_controllers[i].faceButtons[2] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_X;
			m_controllers[i].faceButtons[3] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_Y;
			m_controllers[i].dPadButtons[0] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			m_controllers[i].dPadButtons[1] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
			m_controllers[i].dPadButtons[2] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			m_controllers[i].dPadButtons[3] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			m_controllers[i].bumpers[0] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			m_controllers[i].bumpers[1] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
			m_controllers[i].sticks[0] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			m_controllers[i].sticks[1] = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
			m_controllers[i].startButton = m_controllers[i].gamepad.wButtons & XINPUT_GAMEPAD_START;
			m_controllers[i].backButton = m_controllers[i].gamepad.wButtons * XINPUT_GAMEPAD_BACK;
			m_controllers[i].triggers[0] = m_controllers[i].gamepad.bLeftTrigger;
			m_controllers[i].triggers[1] = m_controllers[i].gamepad.bRightTrigger;
		}
		else
		{
			// Controller is not connected
			m_controllers[i].connected = false;
		}
	}
}

bool ControllerInput::GetButton(BUTTON input, CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return false;

	return m_controllers[int(controllerIndex)].gamepad.wButtons & SHORT(input);

}

bool ControllerInput::GetButtonDown(BUTTON input, CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return false;
	//look into if statements instead of big switch?
	switch (input) {
	case BUTTON::A:	if (m_controllers[int(controllerIndex)].faceButtons[0])		return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_A;
	case BUTTON::B:	if (m_controllers[int(controllerIndex)].faceButtons[1])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_B;
	case BUTTON::X:	if (m_controllers[int(controllerIndex)].faceButtons[2])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_X;
	case BUTTON::Y:	if (m_controllers[int(controllerIndex)].faceButtons[3])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_Y;

	case BUTTON::DUP:		if (m_controllers[int(controllerIndex)].dPadButtons[0])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
	case BUTTON::DRIGHT:	if (m_controllers[int(controllerIndex)].dPadButtons[1])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
	case BUTTON::DDOWN:		if (m_controllers[int(controllerIndex)].dPadButtons[2])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
	case BUTTON::DLEFT:		if (m_controllers[int(controllerIndex)].dPadButtons[3])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;

	case BUTTON::START:		if (m_controllers[int(controllerIndex)].startButton)	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_START;
	case BUTTON::SELECT:	if (m_controllers[int(controllerIndex)].backButton)	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_BACK;

	case BUTTON::LB:	if (m_controllers[int(controllerIndex)].bumpers[0])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
	case BUTTON::LS:	if (m_controllers[int(controllerIndex)].sticks[0])		return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;

	case BUTTON::RB:	if (m_controllers[int(controllerIndex)].bumpers[1])	return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case BUTTON::RS:	if (m_controllers[int(controllerIndex)].sticks[1])		return false;
				return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

	default:	return false;
	}
}

bool ControllerInput::GetButtonUp(BUTTON input, CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return false;
	//look into if statements instead of big switch?
	switch (input) {
	case BUTTON::A:	if (!m_controllers[int(controllerIndex)].faceButtons[0])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_A);
	case BUTTON::B:	if (!m_controllers[int(controllerIndex)].faceButtons[1])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_B);
	case BUTTON::X:	if (!m_controllers[int(controllerIndex)].faceButtons[2])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_X);
	case BUTTON::Y:	if (!m_controllers[int(controllerIndex)].faceButtons[3])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_Y);

	case BUTTON::DUP:		if (!m_controllers[int(controllerIndex)].dPadButtons[0])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
	case BUTTON::DRIGHT:	if (!m_controllers[int(controllerIndex)].dPadButtons[1])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	case BUTTON::DDOWN:		if (!m_controllers[int(controllerIndex)].dPadButtons[2])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	case BUTTON::DLEFT:		if (!m_controllers[int(controllerIndex)].dPadButtons[3])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);

	case BUTTON::START:		if (!m_controllers[int(controllerIndex)].startButton)	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_START);
	case BUTTON::SELECT:	if (!m_controllers[int(controllerIndex)].backButton)	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_BACK);

	case BUTTON::LB:	if (!m_controllers[int(controllerIndex)].bumpers[0])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	case BUTTON::LS:	if (!m_controllers[int(controllerIndex)].sticks[0])		return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);

	case BUTTON::RB:	if (!m_controllers[int(controllerIndex)].bumpers[1])	return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	case BUTTON::RS:	if (!m_controllers[int(controllerIndex)].sticks[1])		return false;
		return !(m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

	default:	return false;
	}

	return false;
}

float ControllerInput::GetLT(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	if (m_controllers[int(controllerIndex)].gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return 0;

	return m_controllers[int(controllerIndex)].gamepad.bLeftTrigger / 255.f;
}

float ControllerInput::GetLX(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbLX;

	if (value < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetLY(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	
	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbLY;

	if (value < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetRT(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	if (m_controllers[int(controllerIndex)].gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return 0;

	return m_controllers[int(controllerIndex)].gamepad.bRightTrigger / 255.f;
}

float ControllerInput::GetRX(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbRX;

	if (value < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetRY(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	
	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbRY;

	if (value < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

//split for raw

int ControllerInput::GetLTRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	return m_controllers[int(controllerIndex)].gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0;
}

int ControllerInput::GetLXRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbLX;
	return change > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetLYRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbLY;
	return change > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetRTRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;

	return m_controllers[int(controllerIndex)].gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0;
}

int ControllerInput::GetRXRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbRX;
	return change > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetRYRaw(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbRY;
	return change > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetLTDown(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected)		return 0;
	if (m_controllers[int(controllerIndex)].triggers[0])	return 0;
	return GetLTRaw(controllerIndex);
}

int ControllerInput::GetRTDown(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	if (m_controllers[int(controllerIndex)].triggers[1])	return 0;
	return GetRTRaw(controllerIndex);
}

int ControllerInput::GetLTUp(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected)		return 0;
	if (m_controllers[int(controllerIndex)].triggers[0])	return 0;
	return GetLTRaw(controllerIndex);
}

int ControllerInput::GetRTUp(CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return 0;
	if (m_controllers[int(controllerIndex)].triggers[1])	return 0;
	return GetRTRaw(controllerIndex);
}

