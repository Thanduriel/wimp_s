#include "camera.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/opengl.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "math/extensions.hpp"
#include "gameplay/elements/ship.hpp"
#include "generators/random.hpp"
namespace Control {

	using namespace ei;

	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 50000.f;

	const float ASPECT_RATIO = 16.f / 9.f;

	Camera g_camera(ei::Vec3(0.f), ei::qidentity(), 0.9f, ASPECT_RATIO);

	const float MOVEMENT_FACTOR = 60.0f;
	const float ROTATION_FACTOR = 7.f;

	Camera::Camera(const Vec3& _position, const Quaternion& _rotation, float _fov, float _aspectRatio)
		: DynamicActor(_position, _rotation),
		m_fov(_fov),
		m_aspectRatio(_aspectRatio),
		m_projection(ei::perspectiveGL(_fov, _aspectRatio, NEAR_PLANE, FAR_PLANE)),
		m_viewProjection(),
		m_mode(Mode::Follow),
		m_targetOffset(Vec3(0.0f, 3.0f, -5.0f)),
		m_currentOffset(Vec3(0.0f, 3.0f, -5.0f)),
		m_speed(0.0f),
		m_acceleration(1.0f)
	{
	}

	void Camera::Process(float _deltaTime)
	{
		switch (m_mode) {
		case Mode::Free:
			ProcessFreeMove(_deltaTime);
			break;
		case Mode::Follow:
			if (m_target)
			{
				SetFollowOrientation(_deltaTime);

				// move to the desired configuration
				m_position = m_targetPosition;
				m_rotation = m_targetRotation;
			}
			break;
		case Mode::MoveTo:
		{
			// move to the desired configuration
			Vec3 dir = m_targetPosition - m_position;
			float l = len(dir);
			float d = m_transitionSpeed * _deltaTime;
			m_position = m_position + (d < l ? dir / l * d : dir);
			// check against new position
			float lSq = lensq(m_targetPosition - m_position);
			if (lSq > 0.1) m_rotation = slerpLess(m_rotation, m_targetRotation, min(1.0f, d / l));
			else m_mode = m_nextMode;

			// the target changes position while the camera moves
			if (m_nextMode == Mode::Follow)
			{
			//	if(m_targetPosition)
				SetFollowOrientation(_deltaTime);
			}
		}
		default:
			break;
		}

		if (m_animationTime > 0.f) ProcessScreenShake(_deltaTime);

		Game::DynamicActor::Process(_deltaTime);

		// update view projection to make sure that it is always up to date
		// the order here is important
		m_view = Mat4x4(m_inverseRotationMatrix) *  translation(-m_position);
		m_viewProjection = m_projection * m_view;

		// update ubo
		UpdateUbo(Graphic::Resources::GetUBO(Graphic::UniformBuffers::CAMERA));
	}

	// ******************************************************************* //
	void Camera::Attach(const Actor& _target)
	{
		m_nextMode = Mode::Follow;
		m_target = &_target;
		MoveTo(m_target->GetPosition() + m_target->GetRotationMatrix() * GetDistance(), m_target->GetRotation());
	}

	void Camera::FixRotation(const ei::Quaternion& _rotation, const ei::Vec3& _position)
	{
		m_nextMode = Mode::Tactical;
		MoveTo(_position, _rotation);
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

	// ******************************************************************* //
	void Camera::Detach()
	{
		m_mode = Mode::Free;
		// center cursor to prevent the first rotation
		IVec2 size = Graphic::Device::GetBackbufferSize() / 2;
		glfwSetCursorPos(Graphic::Device::GetWindow(), size.x, size.y);
	}

	// ******************************************************************* //
	void Camera::MoveTo(const ei::Vec3& _position, const ei::Quaternion& _rotation, float _time)
	{
		m_mode = Mode::MoveTo;
		m_targetRotation = _rotation;
		m_targetPosition = _position;
		m_transitionSpeed = len(m_position - m_targetPosition) / _time;
	}

	// ******************************************************************* //
	using namespace Graphic;
	void Camera::UpdateUbo(UniformBuffer& _ubo)
	{
		_ubo["Projection"] = Vec4(m_projection(0, 0), m_projection(1, 1), m_projection(2, 2), m_projection(2, 3));
		_ubo["InverseProjection"] = Vec4(1.0f / m_projection(0, 0), 1.0f / m_projection(1, 1), 1.0f / m_projection(2, 2), -m_projection(2, 3) / m_projection(2, 2));
		_ubo["NearPlaneSize"] = Vec4(tan(m_fov) * Device::GetAspectRatio(), tan(m_fov), tan(m_fov / 2.f), tan(m_fov / 2.f));
		_ubo["CameraRotation"] = ei::Mat4x4(m_inverseRotationMatrix);
		_ubo["View"] = Mat4x4(m_inverseRotationMatrix) * translation(-m_position);
		_ubo["SignDir"] = sgn(zaxis(m_rotation));
	}

	ei::Vec3 Camera::GetDistance() const
	{
		float factor = ((Game::Ship*)m_target)->GetCurrentSpeed() / ((Game::Ship*)m_target)->GetMaxSpeed();
		Vec3 localAngularVelocity = ((Game::Ship*)m_target)->GetInverseRotationMatrix() * ((Game::Ship*)m_target)->GetAngularVelocity();
		return Vec3(localAngularVelocity.y, m_currentOffset.y - localAngularVelocity.x, m_currentOffset.z);
	}

	void Camera::SetFollowOrientation(float _deltaTime)
	{
		//Set the acceleration relative to the target acceleration
		m_acceleration = 0.005f * ((Game::Ship*)m_target)->GetThrust() / ((Game::Ship*)m_target)->GetMass();

		m_currentOffset = m_targetOffset;
		m_speed = lerp(m_speed, ((Game::Ship*)m_target)->GetCurrentSpeed(), m_acceleration * _deltaTime);
		/*m_speed = 1.0f;*/
		m_currentOffset.z = m_targetOffset.z - (1.0f - (m_speed - ((Game::Ship*)m_target)->GetCurrentSpeed()) / ((Game::Ship*)m_target)->GetMaxSpeed()) * 10.0f;

		m_targetPosition = m_target->GetPosition() + m_target->GetRotationMatrix() * GetDistance();
		m_targetRotation = m_target->GetRotation();
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

	// ******************************************************************* //
	ei::FastFrustum Camera::GetViewFrustum() const
	{
		const float x = tan(m_fov) * FAR_PLANE;
		const float y = x * 1.f / ASPECT_RATIO;

		// normalize for ei asserts
		return ei::FastFrustum(m_position, ei::normalize(ei::transform(Vec3(0.f, 0.f, 1.f), m_rotation)),
			ei::normalize(ei::transform(Vec3(0.f, 1.f, 0.f), m_rotation)), -x, x, -y, y, NEAR_PLANE, FAR_PLANE);
	}

	// ******************************************************************* //
	
	thread_local Generators::RandomGenerator rng(0xBF2CC18);
	void Camera::PlayScreenShake(const ScreenShake& _info)
	{
		// ignore shorter shakes
		if (_info.duration < m_animationTime) return;
		m_animationTime = _info.duration;
		m_rotationShake = rng.Rotation();
		m_screenShakeInfo = _info;
		m_sign = 1.f;
		m_screenShakeInfo.frequency = _info.frequency * ei::PI * 2.f;
	}

	void Camera::ProcessScreenShake(float _deltaTime)
	{
		m_animationTime -= _deltaTime;
		float f = sin(m_animationTime * m_screenShakeInfo.frequency) * m_screenShakeInfo.strength;
	//	m_position += rng.Direction() * 0.2f;
		if (m_sign * f < 0.f)
		{
			m_rotationShake = rng.Rotation();
			m_sign = f;
		}
		m_rotation = slerpLess(m_rotation, m_rotationShake, abs(f) * m_screenShakeInfo.strength * rng.Normal(0.15f));
	}
}