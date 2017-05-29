#include "camera.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"

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
		case Mode::Follow:
			// not tested, do not use
			m_position = Vec3(m_target->GetTransformation() * Vec4(0.f, 0.f, -5.f, 1.f));
			m_rotation = m_target->GetRotation();
			break;
		default:
			break;
		}

		Game::DynamicActor::Process(_deltaTime);
		
		// update view projection to make sure that it is always up to date
		// the order here is important
		m_viewProjection = m_projection * Mat4x4(m_inverseRotationMatrix) *  translation(-m_position);

		// update ubo
		UpdateUbo(Graphic::Resources::GetUBO(Graphic::UniformBuffers::CAMERA));
	}

	// ******************************************************************* //
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

	void Camera::Detach()
	{
		m_mode = Mode::Free;
		// center cursor to prevent the first rotation
		IVec2 size = Graphic::Device::GetBackbufferSize() / 2;
		glfwSetCursorPos(Graphic::Device::GetWindow(), size.x, size.y);
	}

	using namespace Graphic;
	void Camera::UpdateUbo(UniformBuffer& _ubo)
	{
		_ubo["Projection"] = Vec4(m_projection(0, 0), m_projection(1, 1), m_projection(2, 2), m_projection(2, 3));
		_ubo["CameraRotation"] = ei::Mat4x4(m_inverseRotationMatrix);
	}

	// ******************************************************************* //
	ei::Ray Camera::GetRay(const ei::Vec2& _screenSpaceCoordinate) const
	{
		Ray ray;
		// Compute view space position of a point on the near plane
		Vec3 nearPoint = ei::Vec3(1.f / m_projection(0, 0) * _screenSpaceCoordinate[0],
			1.f / m_projection(1, 1) * _screenSpaceCoordinate[1],
			1.0f);	// 1.0 result of inverse project of any coordinate
					// Division by 
		// perform inverse transformation (these are already inverse)
		nearPoint = m_rotationMatrix * nearPoint;
		ray.origin = m_position - nearPoint;
		ray.direction = normalize(nearPoint);
		return ray;
	}
}