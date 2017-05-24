#include "gameplay/core/model.hpp"

namespace Control
{
	using namespace ei;

	/* PlayerController *******************************
	 * Handles the player input and applies movement to the player's model.
	 */
	class PlayerController : public Game::Model
	{
	public:

		PlayerController(const Vec3& _position, const Quaternion& _rotation)
			: Model(_position, _rotation), m_mouseSensitivity(0.5f), m_xAcceleration(20.f, -20.f), m_yAcceleration(20.f, -20.f), m_zAcceleration(20.f, -20.f) {};

		// The basic processing method called once per frame
		void Process(float _deltaTime);

		void MouseMove(float _dx, float _dy);

		// Setter and getter for the mouse sensitivity
		void SetMouseSensitivity(const Vec2& _sensitivity) { m_mouseSensitivity = _sensitivity; };
		const Vec2& GetMouseSensitivity() const { return m_mouseSensitivity; };

	private:

		// The sensitivity of the mouse... duh
		Vec2 m_mouseSensitivity;

		// The acceleration in each direction
		Vec2 m_xAcceleration;
		Vec2 m_yAcceleration;
		Vec2 m_zAcceleration;

		Vec2 m_mouseMovement;
		// Control the input of the player
		void HandleInput(float _deltaTime);
	};

	extern PlayerController* g_player; //global actor controlled by the player
}