#include "playercontroller.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"

namespace Control
{
	using namespace ei;

	PlayerController g_player(Vec3(0.f), qidentity());

	void PlayerController::Process(float _deltaTime)
	{
		// Apply the input to the model
		HandleInput(_deltaTime);
		// Let the base do its work
		Game::DynamicActor::Process(_deltaTime);
	}

	void PlayerController::HandleInput(float _deltaTime)
	{
		Vec3 velocity(0.f);
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
		SetVelocity(m_rotationMatrix * velocity);

		// mouse rotation
		double x, y;
		glfwGetCursorPos(Graphic::Device::GetWindow(), &x, &y);

		if (x + y == 0.f) return;
		IVec2 size = Graphic::Device::GetBackbufferSize() / 2;

		float dx = m_mouseSensitivity.x * _deltaTime * float(x - size.x);
		float dy = m_mouseSensitivity.y * _deltaTime * float(y - size.y);

		glfwSetCursorPos(Graphic::Device::GetWindow(), size.x, size.y);

		// this rotation needs to be inverted
		m_rotation *= Quaternion(dy, dx, 0.f);
	}
}