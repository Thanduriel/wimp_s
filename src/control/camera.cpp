#include "camera.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"

namespace Control {

	using namespace ei;

	Camera g_camera(ei::Vec3(0.f), ei::qidentity(), 0.9f, 16.f / 9.f);

	Camera::Camera(const Vec3& _position, const Quaternion& _rotation, float _fov, float _aspectRatio)
		: DynamicActor(_position, _rotation),
		m_projection(ei::perspectiveGL(_fov, _aspectRatio, 0.1f, 50000.f)),
		m_viewProjection( ),
		m_mode(Mode::Free)
	{
	}

	void Camera::Process(float _deltaTime)
	{
		switch (m_mode){
		case Mode::Free:
			ProcessFreeMove(_deltaTime);
			break;
		default:
			break;
		}

		Game::DynamicActor::Process(_deltaTime);
		
		// update view projection to make sure that it is always up to date
		// the order here is important
		m_viewProjection = m_projection * Mat4x4(m_inverseRotationMatrix) *  translation(-m_position);
	}

	void Camera::ProcessFreeMove(float _deltaTime)
	{
		Vec3 velocity(0.f);
		// movement
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_W))
			velocity[2] += 20.0f;
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_S))
			velocity[2] -= 20.0f;
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_D))
			velocity[0] += 20.0f;
		if (glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_A))
			velocity[0] -= 20.0f;
		SetVelocity(m_rotationMatrix * velocity);

		// mouse rotation
		double x, y;
		glfwGetCursorPos(Graphic::Device::GetWindow(), &x, &y);

		if (x + y == 0.f) return;
		IVec2 size = Graphic::Device::GetBackbufferSize() / 2;

		float dx = 0.5f * _deltaTime * float(x - size.x);
		float dy = 0.5f * _deltaTime * float(y - size.y);
		
		glfwSetCursorPos(Graphic::Device::GetWindow(), size.x, size.y);

		// this rotation needs to be inverted
		m_rotation *= Quaternion(dy, dx, 0.f);
	}
}