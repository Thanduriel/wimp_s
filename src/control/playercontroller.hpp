#pragma once

#include "ei/vector.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/core/actor.hpp"

namespace Game {
	class Model;
	class Actor;
	class Ship;
	class SceneGraph;
}

namespace GameStates {
	class MainHud;
}

namespace Control
{
	using namespace ei;

	struct GameTimeControl
	{
		float m_timeScale;
	};
	/* PlayerController *******************************
	 * Handles the player input and applies movement to the player's model.
	 */
	class PlayerController
	{
	public:

		PlayerController(Game::Ship& _ship, Game::Grid& _grid, 
			Game::Actor& _indicator, GameStates::MainHud& _hud,
			GameTimeControl& _params);

		// The basic processing method called once per frame
		void Process(float _deltaTime);

		void MouseMove(float _dx, float _dy);
		void KeyClick(int _key);
		void KeyDown(int _key, int _modifiers);
		void Scroll(float _dx, float _dy);

		const Game::Ship& GetShip() const { return *m_ship; }

		// Setter and getter for the mouse sensitivity
		void SetMouseSensitivity(const Vec2& _sensitivity) { m_mouseSensitivity = _sensitivity; };
		const Vec2& GetMouseSensitivity() const { return m_mouseSensitivity; };

		static void SetSceneGraph(Game::SceneGraph& _graph) { s_sceneGraph = &_graph; }
	private:
		enum struct TargetingMode
		{
			Normal = 0,
			Tactical = 1
		} m_targetingMode;
		float m_tacticalDirSign; // -1 when the grid.y > camera.y

		// The sensitivity of the mouse... duh
		Vec2 m_mouseSensitivity;

		// the target speed
		float m_targetSpeed;

		// the sensitivity of the target speed slider
		float m_sliderSensitivity;

		Vec2 m_mouseMovement;
		// Control the input of the player
		void HandleInput(float _deltaTime);

		Game::Ship* m_ship;
		Game::Grid& m_grid;
		Game::Actor& m_indicator;
		GameStates::MainHud& m_hud;
		GameTimeControl& m_controlParams;

		Game::Actor::Handle m_focus;

		static Game::SceneGraph* s_sceneGraph;
	};
}