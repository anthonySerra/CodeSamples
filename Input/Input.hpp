//------------------------------------------------------------------------
//
//  Author: Anthony Serra
//
//------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <XInput.h>
#include <map>


#pragma message("Knows about Input.h")
/*
//
// Constants for gamepad buttons
//
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000
*/

// include the DirectX Library files
#pragma comment (lib, "XInput.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define DIMOUSE_LEFTBUTTON   0
#define DIMOUSE_RIGHTBUTTON  1
#define DIMOUSE_MIDDLEBUTTON 2

// XBOX Controller Class Definition
class CXBOXController
{
private:
	CXBOXController(int playerNumber);
	XINPUT_STATE _controllerState, _lastState;
	int _controllerNum;
	XINPUT_STATE Update();
public:
	friend class InputEngine;
	XINPUT_STATE GetState() const {return _controllerState;}
	bool IsPressed(WORD button) const {return (_controllerState.Gamepad.wButtons & button) != 0; }
	bool IsTriggered(WORD button) const {return (_controllerState.Gamepad.wButtons & button) && !(_lastState.Gamepad.wButtons & button);}
	bool IsConnected() const;
	void Vibrate(int leftVal = 0, int rightVal = 0);
};


class InputEngine
{
public:
	InputEngine(HINSTANCE hInstance, HWND hWnd);
	InputEngine();
	~InputEngine();

	static const int NUM_XBOX_CONTROLLERS = 4;

	void Update();
	void Initialize(HINSTANCE hInstance, HWND hWnd);

	CXBOXController *GetXboxController(int ControllerNumber); 
	bool IsPressed(unsigned char keycode);
	bool IsTriggered(unsigned char keycode);

	bool MousePressed(unsigned char mousecode);
	float MouseX();
	float MouseY();

private:

	// function prototypes
	void InitializeDInput(HINSTANCE hInstance, HWND hWnd);    // sets up and initializes DirectInput
	void DetectKeys(void);    // gets the current keys being pressed
	void CleanDInput(void);    // closes DirectInput and releases memory

	// global declarations
	LPDIRECTINPUT8 din;    // the pointer to our DirectInput interface
	LPDIRECTINPUTDEVICE8 dinkeyboard;    // the pointer to the keyboard device
	LPDIRECTINPUTDEVICE8 dinMouse;

	DIMOUSESTATE2 m_MouseState;

	BYTE keystate[256];    // create a static storage for the key-states
	BYTE prevkeystate[256];
	CXBOXController *xBoxControllers[NUM_XBOX_CONTROLLERS];

	void InitMouse(HWND hwnd);

};