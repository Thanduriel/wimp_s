#include "camera.hpp"
#include "graphic/core/device.hpp"

namespace Control {

	using namespace ei;

	Camera g_camera(ei::Vec3(0.f), ei::qidentity(), 0.7f, 16.f / 9.f);

	Camera::Camera(const Vec3& _position, const Quaternion& _rotation, float _fov, float _aspectRatio)
		: DynamicActor(_position, _rotation),
		m_projection(ei::perspectiveGL(_fov, _aspectRatio, 0.5f, 5000.f)),
		m_viewProjection( m_projection * m_transformation )
	{
	}

	void Camera::Process(float _deltaTime)
	{
		Game::DynamicActor::Process(_deltaTime);

		m_viewProjection = m_projection * m_transformation;
	}
}