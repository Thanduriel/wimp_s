#include <algorithm>

#include "input.hpp"
#include <ei/vector.hpp>
#include "graphic/core/device.hpp"
#include "utils/assert.hpp"
#include "GLFW/glfw3.h"
#include "gamestates/gamestate.hpp"
#include "game.hpp"

using namespace std;
using namespace GameStates;

namespace Control {

	static InputManager InputManagerInstance;

	float TimeSinceProgramStart() { return Wimp_s::GetGameTime(); }

	// ********************************************************************* //
	void InputManager::Initialize( GLFWwindow* _window, Jo::Files::MetaFileWrapper::Node& _keyConfig )
	{
		// GLFW setup
		glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode( _window, GLFW_STICKY_KEYS, GL_FALSE );
		glfwSetInputMode( _window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE );

		// Callback setup
		glfwSetCursorPosCallback( _window, CursorPosFun );
		glfwSetCursorEnterCallback( _window, CursorEnterFun );
		glfwSetScrollCallback( _window, ScrollFun );
		glfwSetKeyCallback( _window, KeyFun );
		glfwSetMouseButtonCallback( _window, MouseButtonFun );

		// Avoid invalid pointers
		InputManagerInstance.m_gameState = nullptr;
		InputManagerInstance.m_window = _window;
		
		// Read key mapping
		InputManagerInstance.m_keyMap = new Jo::Files::MetaFileWrapper::Node*[(int)VirtualKey::COUNT];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ACC_FORWARD] = &_keyConfig[string("AccForward")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ACC_BACKWARD] = &_keyConfig[string("AccBackward")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ACC_LEFT] = &_keyConfig[string("AccLeft")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ACC_RIGHT] = &_keyConfig[string("AccRight")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ADJUST_TARGET_ACC] = &_keyConfig[string("AdjustTargetAcc")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::BRAKE] = &_keyConfig[string("Brake")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::FIRE] = &_keyConfig[string("Fire")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::FIREOTHER] = &_keyConfig[string("FireOther")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ROLL_CW] = &_keyConfig[string("RollCW")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::ROLL_CCW] = &_keyConfig[string("RollCCW")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::INVENTORY] = &_keyConfig[string("Inventory")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::SWITCH_TACTICAL] = &_keyConfig[string("SwitchTactical")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::PAUSE] = &_keyConfig[string("Pause")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::LOCK_TARGET] = &_keyConfig[string("LockTarget")];
		InputManagerInstance.m_keyMap[(int)VirtualKey::CYCLE_TARGET] = &_keyConfig[string("CycleTarget")];

		// read other params
		InputManagerInstance.m_mouseSensitivtiy = _keyConfig[string("MouseSensitivity")].Get(1.f);
	}

	// ********************************************************************* //
	void InputManager::Close()
	{
		delete[] InputManagerInstance.m_keyMap;
	}

	// ********************************************************************* //
	void InputManager::Save(Jo::Files::MetaFileWrapper::Node& _keyConfig)
	{
		_keyConfig[string("MouseSensitivity")] = (double)InputManagerInstance.m_mouseSensitivtiy;
	}

	// ********************************************************************* //
	void InputManager::SetGameState( GameStates::GameState* _gameState )
	{
		InputManagerInstance.m_gameState = _gameState;
	}

	// ********************************************************************* //
	bool InputManager::IsKeyPressed(int _key)
	{
		return GLFW_PRESS == glfwGetKey(InputManagerInstance.m_window, _key);
	}

	// ********************************************************************* //
	bool InputManager::IsVirtualKeyPressed( VirtualKey _key )
	{
		Jo::Files::MetaFileWrapper::Node& currentKeys = *InputManagerInstance.m_keyMap[(int)_key];
		for( int i=0; i<currentKeys.Size(); ++i ) {
			int key = (int)currentKeys[i];
			bool isPressed;
			if( key < 32 )
				isPressed = GLFW_PRESS == glfwGetMouseButton( InputManagerInstance.m_window, key );
			else
				isPressed = GLFW_PRESS == glfwGetKey( InputManagerInstance.m_window, key );
			// Stop if any of the buttons from the list is pressed
			if( isPressed ) return true;
		}
		return false;
	}

	// ********************************************************************* //
	bool InputManager::IsVirtualKey( int _key, VirtualKey _vkey )
	{
		Jo::Files::MetaFileWrapper::Node& currentKeys = *InputManagerInstance.m_keyMap[(int)_vkey];
		for( int i=0; i<currentKeys.Size(); ++i ) {
			if( _key == (int)currentKeys[i] ) return true;
		}
		return false;
	}

	// ********************************************************************* //
	void InputManager::CursorPosFun(GLFWwindow* _window, double _x, double _y)
	{
		const double dx = (_x - InputManagerInstance.m_cursorX) * InputManagerInstance.m_mouseSensitivtiy;
		const double dy = (_y - InputManagerInstance.m_cursorY) * InputManagerInstance.m_mouseSensitivtiy;

		int xSize, ySize;
		glfwGetWindowSize(_window, &xSize, &ySize);
		const double vx = std::clamp(InputManagerInstance.m_cursorX + dx, 0.0, (double)xSize);
		const double vy = std::clamp(InputManagerInstance.m_cursorY + dy, 0.0, (double)ySize);
		glfwSetCursorPos(_window, vx, vy);
		// Leave/enter events are handled by m_justEntered (reset position without jump)
		// Compute change of cursor and update its position
//		if( InputManagerInstance.m_justEntered ) dx = 0.0, dy = 0.0;
		InputManagerInstance.m_cursorX = vx;
		InputManagerInstance.m_cursorY = vy;
		InputManagerInstance.m_justEntered = false;
		// Do a mouse move event.
		if( InputManagerInstance.m_gameState )
			InputManagerInstance.m_gameState->MouseMove(InputManagerInstance.m_mouseSensitivtiy * (float)dx, 
				InputManagerInstance.m_mouseSensitivtiy *(float)dy );
	}

	// ********************************************************************* //
	void InputManager::CursorEnterFun(GLFWwindow *, int _entered)
	{
		if( _entered == GL_FALSE )
			InputManagerInstance.m_justEntered = true;
	}

	// ********************************************************************* //
	void InputManager::ScrollFun(GLFWwindow *, double _dx, double _dy)
	{
		InputManagerInstance.m_gameState->Scroll( (float)_dx, (float)_dy );
	}

	// ********************************************************************* //
	void InputManager::KeyFun(GLFWwindow* _window, int _key, int _scanCode, int _action, int _modifiers)
	{
		// Save game state - some events might push new states
		GameState* gameState = InputManagerInstance.m_gameState;

		// React only if somebody is listening
		if( gameState )
		{
			if( _action == GLFW_PRESS )
			{
				InputManagerInstance.m_keyInfos[_key].lastDown = (float)TimeSinceProgramStart();
				gameState->KeyDown( _key, _modifiers );
			} else if( _action == GLFW_RELEASE )
			{
				gameState->KeyRelease( _key );
				// Check for a click
				float now = (float)TimeSinceProgramStart();
				auto& keyInfo = InputManagerInstance.m_keyInfos[_key];
				if( now - keyInfo.lastDown < 0.2f )
				{
					// Simple or double click?
					if( now - keyInfo.lastRelease < 0.25f ) {
						gameState->KeyDoubleClick( _key );
						keyInfo.lastRelease = -1.0f;	// Break tripple clicks
					} else {
						gameState->KeyClick( _key );
						keyInfo.lastRelease = now;
					}
				} else keyInfo.lastRelease = now;
			} else
				// Repeat
				gameState->KeyDown( _key, _modifiers );
		}
	}

	// ********************************************************************* //
	void InputManager::MouseButtonFun(GLFWwindow*, int _key, int _action, int _modifiers)
	{
		// Save game state - some events might push new states
		GameState* gameState = InputManagerInstance.m_gameState;

		// React only if somebody is listening
		if( gameState )
		{
			if( _action == GLFW_PRESS )
			{
				InputManagerInstance.m_keyInfos[_key].lastDown = (float)TimeSinceProgramStart();
				gameState->KeyDown( _key, _modifiers );
			} else if( _action == GLFW_RELEASE )
			{
				gameState->KeyRelease( _key );
				// Check for a click
				float now = (float)TimeSinceProgramStart();
				auto& keyInfo = InputManagerInstance.m_keyInfos[_key];
				if( now - keyInfo.lastDown < 0.15f )
				{
					ei::Vec2 mousePosition = GetCursorPos();
					// Simple or double click?
					if( now - keyInfo.lastRelease < 0.25f
						&& ei::lensq(InputManagerInstance.m_lastClickedPosition - mousePosition) < 5.0f )
					{
						gameState->KeyDoubleClick( _key );
						keyInfo.lastRelease = -1.0f;	// Break triple clicks
					} else {
						gameState->KeyClick( _key );
						keyInfo.lastRelease = now;
						InputManagerInstance.m_lastClickedPosition = mousePosition;
					}
				} else keyInfo.lastRelease = now;
			} else { Assert(false, "Unknown GLFW key action."); }
		}
	}

	// ********************************************************************* //
	std::string InputManager::GetBoundKey(VirtualKey _vkey)
	{
		Jo::Files::MetaFileWrapper::Node& currentKeys = *InputManagerInstance.m_keyMap[(int)_vkey];

		const int key = (int)currentKeys[0];
		std::string s;
		switch (key)
		{
		case GLFW_KEY_LEFT_SHIFT:
			s = "lshift";
			break;
		case GLFW_KEY_TAB:
			s = "tab";
			break;
		default:
			s = KeyToChar(key, 0);
		}

		return std::move(s);
	}

	// ********************************************************************* //
	ei::Vec2 InputManager::GetCursorPos()
	{
		return ei::Vec2( (float)InputManagerInstance.m_cursorX, (float)InputManagerInstance.m_cursorY );
	}

	ei::Vec2 InputManager::GetCursorPosScreenSpace()
	{
		// Get cursor converted to screen coordinates
		ei::Vec2 cursorPos = 2.0f * InputManager::GetCursorPos() / Graphic::Device::GetBackbufferSize() - 1.0f;
		cursorPos[1] = -cursorPos[1];
		return cursorPos;
	}

	void InputManager::SetCursorPos(ei::Vec2 _pos)
	{
		using namespace ei;
		Vec2 v = Vec2(std::clamp(_pos.x, -1.f, 1.f), -std::clamp(_pos.y, -1.f, 1.f));
		v = (v + 1.f) * Graphic::Device::GetBackbufferSize() * 0.5f;
		glfwSetCursorPos(Graphic::Device::GetWindow(), v.x, v.y);

		InputManagerInstance.m_cursorX = v.x;
		InputManagerInstance.m_cursorY = v.y;
		InputManagerInstance.m_justEntered = false;

		if (InputManagerInstance.m_gameState)
		{
			const Vec2 dif = Vec2(v.x - InputManagerInstance.m_cursorX, v.y - InputManagerInstance.m_cursorX);
			InputManagerInstance.m_gameState->MouseMove(dif.x, dif.y);
		}
	}


	// ********************************************************************* //
	void InputManager::SetMouseSensitivtiy(float _sensitivtiy)
	{
		InputManagerInstance.m_mouseSensitivtiy = _sensitivtiy;
	}

	float InputManager::GetMouseSensitivity()
	{
		return InputManagerInstance.m_mouseSensitivtiy;
	}

	// ********************************************************************* //
	char KEY_TO_CHAR[81][3] = {
			{' ', ' ', ' '},		// GLFW_KEY_SPACE
			{0, 0, 0},				// GLFW_KEY_WORLD_1
			{'<', '>', '|'},		// GLFW_KEY_WORLD_2
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{'#', '\'', 0},			// GLFW_KEY_APOSTROPHE
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{',', ';', 0},			// GLFW_KEY_COMMA
			{'-', '_', 0},			// GLFW_KEY_MINUS
			{'.', ':', 0},			// GLFW_KEY_PERIOD
			{'#', '\'', 0},			// GLFW_KEY_SLASH
			{'0', '=', '}'},		// GLFW_KEY_0
			{'1', '!', 0},			// GLFW_KEY_1
			{'2', '"', '�'},		// GLFW_KEY_2
			{'3', '�', '�'},		// GLFW_KEY_3
			{'4', '$', 0},			// GLFW_KEY_4
			{'5', '%', 0},			// GLFW_KEY_5
			{'6', '&', 0},			// GLFW_KEY_6
			{'7', '/', '{'},		// GLFW_KEY_7
			{'8', '(', '['},		// GLFW_KEY_8
			{'9', ')', ']'},		// GLFW_KEY_9
			{0, 0, 0},
			{0, 0, 0},				// GLFW_KEY_SEMICOLON
			{0, 0, 0},
			{'+', '*', '~'},		// GLFW_KEY_EQUAL
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{'a', 'A', 0},			// GLFW_KEY_A
			{'b', 'B', 0},			// GLFW_KEY_B
			{'c', 'C', 0},			// GLFW_KEY_C
			{'d', 'D', 0},			// GLFW_KEY_D
			{'e', 'E', '�'},		// GLFW_KEY_E
			{'f', 'F', 0},			// GLFW_KEY_F
			{'g', 'G', 0},			// GLFW_KEY_G
			{'h', 'H', 0},			// GLFW_KEY_H
			{'i', 'I', 0},			// GLFW_KEY_I
			{'j', 'J', 0},			// GLFW_KEY_J
			{'k', 'K', 0},			// GLFW_KEY_K
			{'l', 'L', 0},			// GLFW_KEY_L
			{'m', 'M', 0},			// GLFW_KEY_M
			{'n', 'N', 0},			// GLFW_KEY_N
			{'o', 'O', 0},			// GLFW_KEY_O
			{'p', 'P', 0},			// GLFW_KEY_P
			{'q', 'Q', 0},			// GLFW_KEY_Q
			{'r', 'R', 0},			// GLFW_KEY_R
			{'s', 'S', 0},			// GLFW_KEY_S
			{'t', 'T', 0},			// GLFW_KEY_T
			{'u', 'U', 0},			// GLFW_KEY_U
			{'v', 'V', 0},			// GLFW_KEY_V
			{'w', 'W', 0},			// GLFW_KEY_W
			{'x', 'X', 0},			// GLFW_KEY_X
			{'y', 'Y', 0},			// GLFW_KEY_Y
			{'z', 'Z', 0},			// GLFW_KEY_Z
			{'�', '?', '\\'},		// GLFW_KEY_LEFT_BRACKET
			{'^', '�', 0},			// GLFW_KEY_BACKSLASH
			{'�', '`', 0},			// GLFW_KEY_RIGHT_BRACKET
			{0, 0, 0},
			{0, 0, 0},				// GLFW_KEY_GRAVE_ACCENT
			{'0', '0', '0'},		// GLFW_KEY_KP_0
			{'1', '1', '1'},		// GLFW_KEY_KP_1
			{'2', '2', '2'},		// GLFW_KEY_KP_2
			{'3', '3', '3'},		// GLFW_KEY_KP_3
			{'4', '4', '4'},		// GLFW_KEY_KP_4
			{'5', '5', '5'},		// GLFW_KEY_KP_5
			{'6', '6', '6'},		// GLFW_KEY_KP_6
			{'7', '7', '7'},		// GLFW_KEY_KP_7
			{'8', '8', '8'},		// GLFW_KEY_KP_8
			{'9', '9', '9'},		// GLFW_KEY_KP_9
			{'.', '.', '.'},		// GLFW_KEY_KP_DECIMAL
			{'/', '/', '/'},		// GLFW_KEY_KP_DIVIDE
			{'*', '*', '*'},		// GLFW_KEY_KP_MULTIPLY
			{'-', '-', '-'},		// GLFW_KEY_KP_SUBTRACT
			{'+', '+', '+'},		// GLFW_KEY_KP_ADD
			{'\n', '\n', '\n'},		// GLFW_KEY_KP_ENTER
			{'=', '=', '='},		// GLFW_KEY_KP_EQUAL
	};

	char KeyToChar( int _key, int _modifiers )
	{
		// Translate modifier key to index position: None, Shift, Alt+Ctrl/AltGr
		_modifiers &= 7;
		if( (_modifiers & 3) == 3 ) return 0;	// Ambiguous Shift + Ctrl
		if( _modifiers == GLFW_MOD_CONTROL ) return 0;
		if( _modifiers & GLFW_MOD_ALT )
		{
			if( _modifiers != 6 ) return 0;
			_modifiers = 2;
		}

		// Bring the relevant key intervals together
		if( _key < 32 || _key > GLFW_KEY_KP_EQUAL ) return 0;
		if( _key > GLFW_KEY_GRAVE_ACCENT )
		{
			if( _key == GLFW_KEY_WORLD_2 ) return KEY_TO_CHAR[2][_modifiers];
			if( _key < GLFW_KEY_ENTER ) return 0;
			if( _key == GLFW_KEY_ENTER ) return '\n';
			if( _key == GLFW_KEY_TAB ) return '\t';
			_key -= 224;
		}
		_key -= 32;

		// No that modifiers and keys are filtered lookup in a table
		return KEY_TO_CHAR[_key][_modifiers];
	}

} // namespace Input