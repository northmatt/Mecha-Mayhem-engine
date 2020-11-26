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

			AButtonPress(i);
			BButtonPress(i);
			XButtonPress(i);
			YButtonPress(i);

			UpDPadPress(i);
			DownDPadPress(i);
			LeftDPadPress(i);
			RightDPadPress(i);

			StartButtonPress(i);
			BackButtonPress(i);

			LBButtonPress(i);
			LTriggerMove(i);
			LStickMove(i);
			LStickPress(i);

			RBButtonPress(i);
			RTriggerMove(i);
			RStickMove(i);
			RStickPress(i);
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
	case BUTTON::A:	if (!m_controllers[int(controllerIndex)].faceButtons[0]) return m_controllers[int(controllerIndex)].gamepad.wButtons & XINPUT_GAMEPAD_A;
				  else return false;
	default:	return m_controllers[int(controllerIndex)].gamepad.wButtons & int(input);
	}
}

bool ControllerInput::GetButtonUp(BUTTON input, CONUSER controllerIndex)
{
	if (!m_controllers[int(controllerIndex)].connected) return false;
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

//split for tests

void ControllerInput::AButtonPress(short ind)
{
	m_controllers[ind].faceButtons[0] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_A;
}

void ControllerInput::BButtonPress(short ind)
{
	m_controllers[ind].faceButtons[1] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_B;
}

void ControllerInput::XButtonPress(short ind)
{
	m_controllers[ind].faceButtons[2] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_X;
}

void ControllerInput::YButtonPress(short ind)
{
	m_controllers[ind].faceButtons[3] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_Y;
}

void ControllerInput::UpDPadPress(short ind)
{
	m_controllers[ind].dPadButtons[0] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
}

void ControllerInput::RightDPadPress(short ind)
{
	m_controllers[ind].dPadButtons[1] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
}

void ControllerInput::DownDPadPress(short ind)
{
	m_controllers[ind].dPadButtons[2] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
}

void ControllerInput::LeftDPadPress(short ind)
{
	m_controllers[ind].dPadButtons[3] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
}

void ControllerInput::LBButtonPress(short ind)
{
	m_controllers[ind].bumpers[0] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
}

void ControllerInput::RBButtonPress(short ind)
{
	m_controllers[ind].bumpers[1] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
}

void ControllerInput::LStickPress(short ind)
{
	m_controllers[ind].lStick[4] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
}

void ControllerInput::RStickPress(short ind)
{
	m_controllers[ind].rStick[4] = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
}

void ControllerInput::StartButtonPress(short ind)
{
	m_controllers[ind].startButton = m_controllers[ind].gamepad.wButtons & XINPUT_GAMEPAD_START;
}

void ControllerInput::BackButtonPress(short ind)
{
	m_controllers[ind].backButton = m_controllers[ind].gamepad.wButtons * XINPUT_GAMEPAD_BACK;
}

void ControllerInput::LTriggerMove(short ind)
{
	m_controllers[ind].triggers[0] = m_controllers[ind].gamepad.bLeftTrigger;
}

void ControllerInput::RTriggerMove(short ind)
{
	m_controllers[ind].triggers[1] = m_controllers[ind].gamepad.bRightTrigger;
}

void ControllerInput::LStickMove(short ind)
{
	m_controllers[ind].lStick[0] = m_controllers[ind].gamepad.sThumbLY > 0;
	m_controllers[ind].lStick[2] = m_controllers[ind].gamepad.sThumbLY < 0;
	m_controllers[ind].lStick[1] = m_controllers[ind].gamepad.sThumbLX > 0;
	m_controllers[ind].lStick[3] = m_controllers[ind].gamepad.sThumbLX < 0;
}

void ControllerInput::RStickMove(short ind)
{
	m_controllers[ind].rStick[0] = m_controllers[ind].gamepad.sThumbRY > 0;
	m_controllers[ind].rStick[2] = m_controllers[ind].gamepad.sThumbRY < 0;
	m_controllers[ind].rStick[1] = m_controllers[ind].gamepad.sThumbRX > 0;
	m_controllers[ind].rStick[3] = m_controllers[ind].gamepad.sThumbRX < 0;
}
