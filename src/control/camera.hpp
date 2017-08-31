#pragma once

#include "ei/3dtypes.hpp"
#include "gameplay/core/actor.hpp"

namespace Graphic{
	class UniformBuffer;
}

namespace Control {

	/* Camera ***************************************
	 * A view with transformation and projection.
	 * It can be freely moved by altering the position and rotation. 
	 */
	class Camera : public Game::DynamicActor
	{
	public:
		enum struct Mode
		{
			Follow, // attached to an actor, mirror movement and rotation?
			Free, // move around freely
			Tactical, // fixed rotation
			MoveTo
		};

		Camera(const ei::Vec3& _position, const ei::Quaternion& _rotation, float _fov, float _aspectRatio);

		void Process(float _deltaTime);

		// The view projection of this camera that transforms worldspace -> cameraspace.
		const ei::Mat4x4& GetViewProjection() const { return m_viewProjection; }
		// Inverse transformation for worldspace -> eyespace
		const ei::Mat4x4& GetView() const { return m_view; }

		// Change the mode to follow _target.
		void Attach(const Game::Actor& _target);

		// Change the mode to tactial preventing changes to the rotation.
		void FixRotation(const ei::Quaternion& _rotation, const ei::Vec3& _position = ei::Vec3(0.f));

		// Change the mode free moving camera.
		void Detach();

		// Update parameters of the global camera ubo with this cameras.
		void UpdateUbo(Graphic::UniformBuffer& _ubo);

		// Returns a ray from the camera that goes through the given point.
		ei::Ray GetRay(const ei::Vec2& _screenSpaceCoordinate) const;

		// Gets the distance to the target ship
		ei::Vec3 GetDistance() const;

		float GetFov() const { return m_fov; };

		float GetAspectRatio() const { return m_aspectRatio; };
	private:
		void ProcessFreeMove(float _deltaTime);
		void MoveTo(const ei::Vec3& _position, const ei::Quaternion& _rotation, float _time = 0.6f);
		void SetFollowOrientation(float _deltaTime);

		float m_fov;
		ei::Mat4x4 m_projection;
		ei::Mat4x4 m_viewProjection;
		ei::Mat4x4 m_view;
		float m_aspectRatio;

		ei::Vec3 m_targetPosition;
		ei::Quaternion m_targetRotation;
		float m_transitionSpeed;
		Mode m_mode;
		Mode m_nextMode; // mode after the target position has been reached
		const Game::Actor* m_target;
		ei::Vec3 m_targetOffset;
		ei::Vec3 m_currentOffset;
		float m_speed;
		float m_acceleration;
	};

	extern Camera g_camera; // global default camera used for scene rendering
}