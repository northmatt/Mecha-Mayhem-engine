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

		auto& current = m_controllers[i];

		current.lastConnected = current.connected;

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			current.connected = true;
			current.gamepad = state.Gamepad;
		}
		else
		{
			// Controller is not connected
			current.connected = false;
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

		auto& current = m_controllers[i];

		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			current.connected = true;

			//updating for the getdown functions
			current.faceButtons[0]	= current.gamepad.wButtons & XINPUT_GAMEPAD_A;
			current.faceButtons[1]	= current.gamepad.wButtons & XINPUT_GAMEPAD_B;
			current.faceButtons[2]	= current.gamepad.wButtons & XINPUT_GAMEPAD_X;
			current.faceButtons[3]	= current.gamepad.wButtons & XINPUT_GAMEPAD_Y;
			current.dPadButtons[0]	= current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			current.dPadButtons[1]	= current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
			current.dPadButtons[2]	= current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			current.dPadButtons[3]	= current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			current.bumpers[0]		= current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			current.bumpers[1]		= current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
			current.sticks[0]		= current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			current.sticks[1]		= current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
			current.startButton		= current.gamepad.wButtons & XINPUT_GAMEPAD_START;
			current.backButton		= current.gamepad.wButtons * XINPUT_GAMEPAD_BACK;
			current.triggers[0]		= current.gamepad.bLeftTrigger;
			current.triggers[1]		= current.gamepad.bRightTrigger;
		}
		else if (current.connected)
		{
			// Controller is not connected (only need to do once connected)
			current.connected = false;

			current.faceButtons[0]	= false;
			current.faceButtons[1]	= false;
			current.faceButtons[2]	= false;
			current.faceButtons[3]	= false;
			current.dPadButtons[0]	= false;
			current.dPadButtons[1]	= false;
			current.dPadButtons[2]	= false;
			current.dPadButtons[3]	= false;
			current.bumpers[0]		= false;
			current.bumpers[1]		= false;
			current.sticks[0]		= false;
			current.sticks[1]		= false;
			current.startButton		= false;
			current.backButton		= false;
			current.triggers[0]		= false;
			current.triggers[1]		= false;
		}
	}
}

bool ControllerInput::ControllerDisconnected(CONUSER controllerIndex)
{
	if (controllerIndex == CONUSER::NONE)	return false;

	auto& current = m_controllers[int(controllerIndex)];

	if (!current.connected) {
		return current.lastConnected != current.connected;
	}

	return false;
}

bool ControllerInput::ControllerConnected(CONUSER controllerIndex)
{
	if (controllerIndex == CONUSER::NONE)	return false;

	auto& current = m_controllers[int(controllerIndex)];

	if (current.connected) {
		return current.lastConnected != current.connected;
	}

	return false;
}

bool ControllerInput::GetButton(BUTTON input, CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return false;

	return m_controllers[int(controllerIndex)].gamepad.wButtons & SHORT(input);

}

bool ControllerInput::GetButtonDown(BUTTON input, CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return false;

	auto& current = m_controllers[int(controllerIndex)];

	//look into if statements instead of big switch?
	switch (input) {
	case BUTTON::A:	if (current.faceButtons[0])		return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_A;
	case BUTTON::B:	if (current.faceButtons[1])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_B;
	case BUTTON::X:	if (current.faceButtons[2])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_X;
	case BUTTON::Y:	if (current.faceButtons[3])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_Y;

	case BUTTON::DUP:		if (current.dPadButtons[0])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
	case BUTTON::DRIGHT:	if (current.dPadButtons[1])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
	case BUTTON::DDOWN:		if (current.dPadButtons[2])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
	case BUTTON::DLEFT:		if (current.dPadButtons[3])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;

	case BUTTON::START:		if (current.startButton)	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_START;
	case BUTTON::SELECT:	if (current.backButton)	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_BACK;

	case BUTTON::LB:	if (current.bumpers[0])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
	case BUTTON::LS:	if (current.sticks[0])		return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;

	case BUTTON::RB:	if (current.bumpers[1])	return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case BUTTON::RS:	if (current.sticks[1])		return false;
				return current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

	default:	return false;
	}
}

bool ControllerInput::GetButtonUp(BUTTON input, CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return false;

	auto& current = m_controllers[int(controllerIndex)];

	//look into if statements instead of big switch?
	switch (input) {
	case BUTTON::A:	if (!current.faceButtons[0])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_A);
	case BUTTON::B:	if (!current.faceButtons[1])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_B);
	case BUTTON::X:	if (!current.faceButtons[2])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_X);
	case BUTTON::Y:	if (!current.faceButtons[3])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_Y);

	case BUTTON::DUP:		if (!current.dPadButtons[0])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
	case BUTTON::DRIGHT:	if (!current.dPadButtons[1])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	case BUTTON::DDOWN:		if (!current.dPadButtons[2])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	case BUTTON::DLEFT:		if (!current.dPadButtons[3])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);

	case BUTTON::START:		if (!current.startButton)	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_START);
	case BUTTON::SELECT:	if (!current.backButton)	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_BACK);

	case BUTTON::LB:	if (!current.bumpers[0])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	case BUTTON::LS:	if (!current.sticks[0])		return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);

	case BUTTON::RB:	if (!current.bumpers[1])	return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	case BUTTON::RS:	if (!current.sticks[1])		return false;
		return !(current.gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);

	default:	return false;
	}

	return false;
}

float ControllerInput::GetLT(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	if (m_controllers[int(controllerIndex)].gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return 0;

	return m_controllers[int(controllerIndex)].gamepad.bLeftTrigger / 255.f;
}

float ControllerInput::GetLX(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbLX;

	if (value < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetLY(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;
	
	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbLY;

	if (value < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetRT(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	if (m_controllers[int(controllerIndex)].gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return 0;

	return m_controllers[int(controllerIndex)].gamepad.bRightTrigger / 255.f;
}

float ControllerInput::GetRX(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbRX;

	if (value < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

float ControllerInput::GetRY(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;
	
	SHORT value = m_controllers[int(controllerIndex)].gamepad.sThumbRY;

	if (value < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		value > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return 0;

	return value / 32767.f;
}

//split for raw

int ControllerInput::GetLTRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	return m_controllers[int(controllerIndex)].gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0;
}

int ControllerInput::GetLXRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbLX;
	return change > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetLYRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbLY;
	return change > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetRTRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	return m_controllers[int(controllerIndex)].gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0;
}

int ControllerInput::GetRXRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbRX;
	return change > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetRYRaw(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	SHORT change = m_controllers[int(controllerIndex)].gamepad.sThumbRY;
	return change > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? 1 : (change < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ? -1 : 0);
}

int ControllerInput::GetLTDown(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex))		return 0;

	if (m_controllers[int(controllerIndex)].triggers[0])	return 0;
	return GetLTRaw(controllerIndex);
}

int ControllerInput::GetRTDown(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	if (m_controllers[int(controllerIndex)].triggers[1])	return 0;
	return GetRTRaw(controllerIndex);
}

int ControllerInput::GetLTUp(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex))		return 0;

	if (m_controllers[int(controllerIndex)].triggers[0])	return 0;
	return GetLTRaw(controllerIndex);
}

int ControllerInput::GetRTUp(CONUSER controllerIndex)
{
	if (TestCON(controllerIndex)) return 0;

	if (m_controllers[int(controllerIndex)].triggers[1])	return 0;
	return GetRTRaw(controllerIndex);
}

