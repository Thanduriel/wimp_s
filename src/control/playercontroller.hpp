#pragma once

#include "ei/vector.hpp"
#include "gameplay/elements/grid.hpp"
#include "gameplay/core/actor.hpp"
#include "controller.hpp"

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
	class PlayerController : public Controller
	{
	public:

		PlayerController(Game::Ship& _ship, GameStates::MainHud& _hud,
			GameTimeControl& _params);

		// The basic processing method called once per frame
		void ProcessComponent(float _deltaTime, const Game::SceneGraph& _sceneGraph) override;

		void OnDamageTaken(float _amount, Game::Actor& _source, Game::DamageType _type) override;

		void MouseMove(float _dx, float _dy);
		void KeyClick(int _key);
		void KeyDown(int _key, int _modifiers);
		void Scroll(float _dx, float _dy);

		void SetFocus(Game::Ship* _actor);
		static bool HAS_AIM_ASSIST;
	private:
		enum struct TargetingMode
		{
			Normal = 0,
			Tactical = 1
		} m_targetingMode;

		void SwitchTargetingMode(TargetingMode _newMode);
		void SideThrust(const ei::Vec3& _velocity);

		float m_tacticalDirSign; // -1 when the grid.y > camera.y

		// the target speed
		float m_targetSpeed;

		float m_sideThrustTimer;
		bool m_isSideThrustReady;

		// the sensitivity of the target speed slider
		float m_sliderSensitivity;

		Vec2 m_mouseMovement;
		// Control the input of the player
		void HandleInput(float _deltaTime);

		void UpdateAimAssist();
		void LookForTarget(const Game::SceneGraph& _sceneGraph);

		// helpers for special weapon placement
		Game::Grid& m_referenceGrid;
		ei::Vec3 m_cameraOffset;

		GameTimeControl& m_controlParams;

		Game::Actor::Handle m_focus;
		bool m_lookForTarget;
	};
}