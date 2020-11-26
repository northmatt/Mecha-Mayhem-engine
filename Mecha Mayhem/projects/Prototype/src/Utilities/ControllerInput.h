#pragma once
#include <iostream>
#include <Windows.h>
#include <xinput.h>

enum class CONUSER
{
	ONE = 0,
	TWO,
	THREE,
	FOUR
};

enum class BUTTON
{
	/*
	A = 0,
	B,
	X,
	Y,
	DUP,	//DPAD
	DRIGHT,	//DPAD
	DDOWN,	//DPAD
	DLEFT,	//DPAD
	START,
	SELECT,	//aka back
	LB,		//Left Bumper/button
	RB,		//Right Bumper/button
	LS,		//Left Stick press
	RS		//Right Stick press
	*/
	A = XINPUT_GAMEPAD_A,
	B = XINPUT_GAMEPAD_B,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y,
	DUP = XINPUT_GAMEPAD_DPAD_UP,		//DPAD
	DRIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,	//DPAD
	DDOWN = XINPUT_GAMEPAD_DPAD_DOWN,	//DPAD
	DLEFT = XINPUT_GAMEPAD_DPAD_LEFT,	//DPAD
	START = XINPUT_GAMEPAD_START,
	SELECT = XINPUT_GAMEPAD_BACK,		//aka back
	LB = XINPUT_GAMEPAD_LEFT_SHOULDER,	//Left Bumper/button
	RB = XINPUT_GAMEPAD_RIGHT_SHOULDER,	//Right Bumper/button
	LS = XINPUT_GAMEPAD_LEFT_THUMB,		//Left Stick press
	RS = XINPUT_GAMEPAD_RIGHT_THUMB		//Right Stick press
};

class ControllerInput abstract
{
public:
	//so that GetButtonDown works
	static void ControllerRefresh();
	//update the stored list
	static void ControllerUpdate();

	static bool GetButton(BUTTON input, CONUSER controllerIndex);
	//dont work rn, want bit stuff
	static bool GetButtonDown(BUTTON input, CONUSER controllerIndex);
	//dont work rn, want bit stuff
	static bool GetButtonUp(BUTTON input, CONUSER controllerIndex);

	//Left Trigger
	static float GetLT(CONUSER controllerIndex);
	//Left Stick X
	static float GetLX(CONUSER controllerIndex);
	//Left Stick Y
	static float GetLY(CONUSER controllerIndex);

	//Right Trigger
	static float GetRT(CONUSER controllerIndex);
	//Right Stick X
	static float GetRX(CONUSER controllerIndex);
	//Right Stick Y
	static float GetRY(CONUSER controllerIndex);

	//Left Trigger pressed?
	static int GetLTRaw(CONUSER controllerIndex);
	//Left Stick X pressed?
	static int GetLXRaw(CONUSER controllerIndex);
	//Left Stick Y pressed?
	static int GetLYRaw(CONUSER controllerIndex);

	//Right Trigger pressed?
	static int GetRTRaw(CONUSER controllerIndex);
	//Right Stick X pressed?
	static int GetRXRaw(CONUSER controllerIndex);
	//Right Stick Y pressed?
	static int GetRYRaw(CONUSER controllerIndex);

private:
	//Face Button presses
	static void AButtonPress(short ind);
	static void BButtonPress(short ind);
	static void XButtonPress(short ind);
	static void YButtonPress(short ind);

	//DPad Button Presses
	static void UpDPadPress(short ind);
	static void RightDPadPress(short ind);
	static void DownDPadPress(short ind);
	static void LeftDPadPress(short ind);

	//Bumpers
	static void LBButtonPress(short ind);
	static void RBButtonPress(short ind);

	//Analog Stick Presses
	static void LStickPress(short ind);
	static void RStickPress(short ind);

	static void StartButtonPress(short ind);
	static void BackButtonPress(short ind);

	static void LTriggerMove(short ind);
	static void RTriggerMove(short ind);

	static void LStickMove(short ind);
	static void RStickMove(short ind);

	struct Controller
	{
		XINPUT_GAMEPAD gamepad;
		bool connected = false;
		//fills with 0 so false
		//a/b/x/y
		bool faceButtons[4] = {};
		
		//up/right/down/left
		bool dPadButtons[4] = {};

		bool startButton = 0;
		//not in Xinput
		bool backButton = 0;

		//left/right
		bool bumpers[2] = {};	
		bool triggers[2] = {};

		//up/right/down/left/press
		//clockwise
		bool lStick[5] = {};
		bool rStick[5] = {};
	};
	
	static Controller m_controllers[XUSER_MAX_COUNT];
};
