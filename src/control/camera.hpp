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
			Tactical // fixed rotation
		};

		Camera(const ei::Vec3& _position, const ei::Quaternion& _rotation, float _fov, float _aspectRatio);

		void Process(float _deltaTime);

		// The view projection of this camera that transforms worldspace -> cameraspace.
		const ei::Mat4x4& GetViewProjection() const { return m_viewProjection; }

		void Attach(const Game::Actor& _target) { m_mode = Mode::Follow; m_target = &_target; };
		void FixRotation(const ei::Quaternion& _rotation) { m_mode = Mode::Tactical; SetRotation(_rotation); }
		void UnfixRotation() { m_mode = Mode::Follow; };
		void Detach();

		void UpdateUbo(Graphic::UniformBuffer& _ubo);

		// Returns a ray from the camera that goes through the given point.
		ei::Ray GetRay(const ei::Vec2& _screenSpaceCoordinate) const;
	private:
		void ProcessFreeMove(float _deltaTime);

		float m_fov;
		ei::Mat4x4 m_projection;
		ei::Mat4x4 m_viewProjection;

		Mode m_mode;
		const Game::Actor* m_target;
		const float m_distanceToTarget;
	};

	extern Camera g_camera; // global default camera used for scene rendering
}