#include "ControllerInput.h"
//Face Button presses
void AButtonPress();
void BButtonPress();
void XButtonPress();
void YButtonPress();

//DPad Button Presses
void UpDPadPress();
void RightDPadPress();
void DownDPadPress();
void LeftDPadPress();

//Bumpers
void RBButtonPress();
void LBButtonPress();

//Analog Stick Presses
void LStickPress();
void RStickPress();

void StartButtonPress();
void BackButtonPress();

void LTriggerMove(float amount);
void RTriggerMove(float amount);

void LStickMove(float x, float y);
void RStickMove(float x, float y);


void ControllerInput::ControllerUpdate() 
{
    //This basically checks all the controllers, and if they are plugged in
    DWORD dwResult;
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        // Simply get the state of the controller from XInput.
        dwResult = XInputGetState(i, &state);

        if (dwResult == ERROR_SUCCESS)
        {
            // Controller is connected

            //
            //face buttons
            //
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_A)
            {
                AButtonPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_B)
            {
                BButtonPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_X)
            {
                XButtonPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_Y)
            {
                YButtonPress();
            }
            //
            //dpad
            //
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_UP)
            {
                UpDPadPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_RIGHT)
            {
                RightDPadPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_DOWN)
            {
                DownDPadPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_DPAD_LEFT)
            {
                LeftDPadPress();
            }
            //
            //Shoulder Bumpers
            //
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_LEFT_SHOULDER)
            {
                LBButtonPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_RIGHT_SHOULDER)
            {
                RBButtonPress();
            }
            //
            //Analog click
            //
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_LEFT_THUMB)
            {
                LStickPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_RIGHT_THUMB)
            {
                RStickPress();
            }
            //
            //Start/Back Button Presses
            //
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_START)
            {
                StartButtonPress();
            }
            if (state.Gamepad.wButtons == XINPUT_GAMEPAD_BACK)
            {
                BackButtonPress();
            }


            //
            //Analog Stick Movement
            //
            float leftTrigger = (float)state.Gamepad.bLeftTrigger / 255;
            float rightTrigger = (float)state.Gamepad.bRightTrigger / 255;
            if (leftTrigger > 0.1)
            {
                LTriggerMove(leftTrigger);
            }
            if (rightTrigger > 0.1)
            {
                RTriggerMove(rightTrigger);
            }
            float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
            float normLY = fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767);
            //std::cout << normLX << " " << normLY  << "\n";
            if (normLX > 0.1f || normLX < -0.1f)
            {
                LStickMove(normLX, normLY);
            }
            float normRX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
            float normRY = fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767);
            
            if (normRX > 0.1f || normRX < -0.1f)
            {
                RStickMove(normRX, normRY);
            }

        }
        else
        {
            // Controller is not connected
        }
    }
}

void AButtonPress()
{
    std::cout << "A";
}

void BButtonPress()
{
    std::cout << "B";
}

void XButtonPress()
{
    std::cout << "X";
}

void YButtonPress()
{
    std::cout << "Y";
}

void UpDPadPress()
{
    std::cout << "Up";
}

void RightDPadPress()
{
    std::cout << "Right";
}

void DownDPadPress()
{
    std::cout << "Down";
}

void LeftDPadPress()
{
    std::cout << "Left";
}

void RBButtonPress()
{
    std::cout << "RB";
}

void LBButtonPress()
{
    std::cout << "LB";
}

void LStickPress()
{
    std::cout << "L3/left stick press";
}

void RStickPress()
{
    std::cout << "R3/Right stick press";
}

void StartButtonPress()
{
    std::cout << "Start";
}

void BackButtonPress()
{
    std::cout << "Back";
}

void LTriggerMove(float amount)
{
    std::cout << amount <<"\n";
}

void RTriggerMove(float amount)
{
    std::cout << amount << "\n";
}

void LStickMove(float x, float y)
{
    std::cout << x << " " << y << "\n";
}

void RStickMove(float x, float y)
{
    std::cout << x << " " << y << "\n";
}
