#include "playercontroller.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"

namespace Control
{
	using namespace ei;

	PlayerController* g_player;

	void PlayerController::Process(float _deltaTime)
	{
		// Apply the input to the model
		HandleInput(_deltaTime);
		// Let the base do its work
		Game::DynamicActor::Process(_deltaTime);
	}

	void PlayerController::MouseMove(float _dx, float _dy)
	{
		m_mouseMovement = Vec2(_dx, _dy);
	}

	// ************************************************************ //
	void PlayerController::HandleInput(float _deltaTime)
	{
	/*	Vec3 velocity(0.f);
		// basic movement (no intertia yet)
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
			velocity[2] += m_zAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
			velocity[2] += m_zAcceleration[1];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
			velocity[0] += m_xAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
			velocity[0] += m_xAcceleration[1];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_SPACE))
			velocity[1] += m_yAcceleration[0];
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_LEFT_SHIFT))
			velocity[1] += m_yAcceleration[1];
		SetVelocity(m_rotationMatrix * velocity);*/

		// mouse rotation
		if (m_mouseMovement.x + m_mouseMovement.y == 0.f) return;

		float dx = m_mouseSensitivity.x * _deltaTime * m_mouseMovement.x;
		float dy = m_mouseSensitivity.y * _deltaTime * m_mouseMovement.y;

		// this rotation needs to be inverted
		Rotate(Quaternion(dx, dy, 0.f));
		m_mouseMovement = Vec2(0.f);
	}
}