#include "ei/vector.hpp"

namespace Game {
	class Grid;
	class Model;
	class Actor;
}

namespace Control
{
	using namespace ei;

	/* PlayerController *******************************
	 * Handles the player input and applies movement to the player's model.
	 */
	class PlayerController
	{
	public:

		PlayerController(Game::Model& _model, Game::Grid& _grid, Game::Actor& _indicator);

		// The basic processing method called once per frame
		void Process(float _deltaTime);

		void MouseMove(float _dx, float _dy);
		void KeyClick(int _key);
		void Scroll(float _dx, float _dy);

		const Game::Model& GetShip() const { return *m_model; }

		// Setter and getter for the mouse sensitivity
		void SetMouseSensitivity(const Vec2& _sensitivity) { m_mouseSensitivity = _sensitivity; };
		const Vec2& GetMouseSensitivity() const { return m_mouseSensitivity; };

	private:
		enum struct TargetingMode
		{
			Normal = 0,
			Tactical = 1
		} m_targetingMode;
		float m_tacticalDirSign; // -1 when the grid.y > camera.y

		// The sensitivity of the mouse... duh
		Vec2 m_mouseSensitivity;

		// The thrust force in each direction
		Vec2 m_xForce;
		Vec2 m_yForce;
		Vec2 m_zForce;

		// The friction in every direction from 0 to 1/_deltaTime
		float m_friction;

		// the target rotation to slerp towards
		Quaternion m_targetRotation;

		Vec2 m_mouseMovement;
		// Control the input of the player
		void HandleInput(float _deltaTime);

		Game::Model* m_model;
		Game::Grid& m_grid;
		Game::Actor& m_indicator;
	};
}