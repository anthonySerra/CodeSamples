//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
#include "Input.hpp"

#pragma message("Building Input Engine")

InputEngine::InputEngine(HINSTANCE hInstance, HWND hWnd)
{
	Initialize(hInstance, hWnd);

}

InputEngine::InputEngine()
{

}

InputEngine::~InputEngine()
{
	CleanDInput();
	for( int i = 0; i < NUM_XBOX_CONTROLLERS; i++ )
	{
		delete xBoxControllers[i];
	}
}

void InputEngine::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	InitializeDInput(hInstance, hWnd);
	for( int i = 0; i < NUM_XBOX_CONTROLLERS; i++ )
	{
		xBoxControllers[i] = new CXBOXController(i+1);
	}
}

void InputEngine::InitializeDInput(HINSTANCE hInstance, HWND hWnd)
{
	// create the DirectInput interface
	DirectInput8Create(hInstance,    // the handle to the application
		DIRECTINPUT_VERSION,    // the compatible version
		IID_IDirectInput8,    // the DirectInput interface version
		(void**)&din,    // the pointer to the interface
		NULL);    // COM stuff, so we'll set it to NULL

	// create the keyboard device
	din->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
		&dinkeyboard,    // the pointer to the device interface
		NULL);    // COM stuff, so we'll set it to NULL

	dinkeyboard->SetDataFormat(&c_dfDIKeyboard); // set the data format to keyboard format


	// set the control you will have over the keyboard
	dinkeyboard->SetCooperativeLevel(hWnd,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);


	InitMouse(hWnd);


}


void InputEngine::DetectKeys()
{
	dinkeyboard->Acquire();    // get access if we don't have it already

	dinkeyboard->GetDeviceState(256, (LPVOID)keystate);    // fill keystate with values
}


void InputEngine::CleanDInput()
{
	dinkeyboard->Unacquire();    // make sure the keyboard is unacquired
	dinkeyboard->Release();
	if(dinMouse != NULL)
	{
		dinMouse->Unacquire();
		dinMouse->Release();
		dinMouse = NULL;
	}
	din->Release();    // close DirectInput before exiting
}

void InputEngine::InitMouse(HWND hwnd)
{
	if(FAILED(din->CreateDevice(GUID_SysMouse,
		&dinMouse,
		NULL)))
	{
		MessageBox(NULL,"failed to create mouse device", "", MB_OK);
		return;
	}
	dinMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(dinMouse->SetCooperativeLevel(hwnd,
		DISCL_BACKGROUND | 
		DISCL_NONEXCLUSIVE)))
	{
		MessageBox(hwnd,
			"SetCooperativeLevel() failed!",
			"InitMouse()",
			MB_OK);
		return ;
	}
	if(FAILED(dinMouse->Acquire()))
	{
		MessageBox(hwnd,
			"Acquire() failed!",
			"InitMouse()",
			MB_OK);
		return;
	}
	
}

CXBOXController *InputEngine::GetXboxController(int ControllerNumber)
{
	if( ControllerNumber > NUM_XBOX_CONTROLLERS || ControllerNumber < 1 )
		return NULL;

	return xBoxControllers[ControllerNumber-1];
}

void InputEngine::Update()
{
	if(DIERR_INPUTLOST == dinMouse->GetDeviceState(sizeof(m_MouseState),
		(LPVOID)&m_MouseState))
	{
		dinMouse->Acquire();
	}
	
	for( int i = 0; i < NUM_XBOX_CONTROLLERS; i++ )
		xBoxControllers[i]->Update();

	for( short i = 0; i < 256; ++i )
		prevkeystate[i] = keystate[i];
	DetectKeys();
}

bool InputEngine::IsPressed(unsigned char keycode)
{
	return (keystate[keycode] & 0x80) != 0;
}

bool InputEngine::IsTriggered(unsigned char keycode)
{
	return (keystate[keycode] & 0x80) && !(prevkeystate[keycode] & 0x80);
}

bool InputEngine::MousePressed(unsigned char mousecode)
{
	if(m_MouseState.rgbButtons[mousecode] & 0x80)
	{
		return true;
	}

	return false;
}

float InputEngine::MouseX()
{
	return static_cast<float>(m_MouseState.lX);
}

float InputEngine::MouseY()
{
	return static_cast<float>(m_MouseState.lY);
}

CXBOXController::CXBOXController(int playerNumber)
{
	// Set the Controller Number
	_controllerNum = playerNumber - 1;
}

XINPUT_STATE CXBOXController::Update()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	_lastState = _controllerState;
	XInputGetState(_controllerNum, &_controllerState);

	return _controllerState;
}

bool CXBOXController::IsConnected() const
{
	// Zeroise the state
	//ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	XINPUT_STATE temp;
	DWORD Result = XInputGetState(_controllerNum, &temp);

	if(Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CXBOXController::Vibrate(int leftVal, int rightVal)
{
	// Create a Vibraton State
	XINPUT_VIBRATION Vibration;

	// Zeroise the Vibration
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;

	// Vibrate the controller
	XInputSetState(_controllerNum, &Vibration);
}