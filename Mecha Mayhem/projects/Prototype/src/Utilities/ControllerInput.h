#pragma once
#include <iostream>
#include <Windows.h>
#include <xinput.h>


class ControllerInput abstract
{
public:
	static void ControllerUpdate();

private:
	struct ControllerPress
	{
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
	
	ControllerPress pressed[XUSER_MAX_COUNT] = {};
};
