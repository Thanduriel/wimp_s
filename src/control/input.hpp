#pragma once

#include "predeclarations.hpp"
#include <ei/vector.hpp>
//#include "../../dependencies/glfw-3.0.3/include/GLFW/glfw3.h"
#include <jofilelib.hpp>
#include <unordered_map>

namespace GameStates {
	class GameState;
}

namespace Control {

	/// \brief The input manager passes inputs to the current set game state
	///		and manages some input state options.
	/// \details The manager must be singleton to allow correct callback setup.
	class InputManager
	{
	public:
		/// \brief Initializes callbacks and settings for GLFW
		/// \details This input manager only allows single window input systems.
		static void Initialize( GLFWwindow* _window, Jo::Files::MetaFileWrapper::Node& _keyConfig );

		/// \brief Release all resources
		static void Close();

		/// \brief Switch which game state gets the input.
		/// \param [in] _gameState null to disable input handling or a
		///		game state which should receive the input events.
		static void SetGameState( GameStates::GameState* _gameState );

		/// \brief Test if a  GLFW key or button is currently pressed.
		static bool IsKeyPressed( int _key );

		/// \brief Test if a mapped key or button is currently pressed.
		static bool IsVirtualKeyPressed( VirtualKey _key );

		/// \brief Check if a physical key maps to a certain virtual key.
		static bool IsVirtualKey( int _key, VirtualKey _vkey );

		/// \brief The cursors position in pixels inside the window.
		static ei::Vec2 GetCursorPos();

		/// \brief The cursors position in screen space coordinates [-1,1]² inside the window.
		static ei::Vec2 GetCursorPosScreenSpace();
	private:
		GameStates::GameState* m_gameState;
		GLFWwindow* m_window;		///< The one reference window
		Jo::Files::MetaFileWrapper::Node** m_keyMap;		///< Faster access to the correct nodes of the config file.

		double m_cursorX;	///< Last known position of the cursor
		double m_cursorY;	///< Last known position of the cursor
		ei::Vec2 m_lastClickedPosition;	///< Used for double click detection for mouse buttons
		bool m_justEntered;	///< Has the cursor (re)entered the window since last mouse move.

		static void CursorPosFun(GLFWwindow *, double, double);
		static void CursorEnterFun(GLFWwindow *, int);
		static void ScrollFun(GLFWwindow *, double, double);
		static void KeyFun(GLFWwindow* _window, int _key, int _scanCode, int _action, int _modifiers);
		static void MouseButtonFun(GLFWwindow*, int _key, int _action, int _modifiers);

		/// \brief Holds information about the past for each key.
		struct KeyInfo
		{
			float lastDown;		///< Time marker used to find click events
			float lastRelease;	///< Time marker used to find double click events
		};
		std::unordered_map<int, KeyInfo> m_keyInfos;	///< Brief holds the information struct for each key which was pressed at least once.
	};

	// Ingame hard coded keys. They are mapped by a configuration file.
	// It is even possible that the keys are mapped to more than one key.
	enum struct VirtualKey {
		ACC_FORWARD,
		ACC_BACKWARD,
		ACC_LEFT,
		ACC_RIGHT,
		ADJUST_TARGET_ACC,
		FIRE,
		ROLL_CW,
		ROLL_CCW,
		INVENTORY,
		COUNT
	};

	/// \brief Takes the glfw input and converts it to a printable character
	///		if possible.
	///	\details Currently this is a German keyboard implementation. As soon as
	///		glfw supports converting keys this will be used instead.
	/// \return The ASCII character for the pressed key+modifiers or 0 if it
	///		was a function key.
	char KeyToChar( int _key, int _modifiers );
} // namespace Input