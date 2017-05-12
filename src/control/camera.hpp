#pragma once

#include "gameplay/core/actor.hpp"

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
	private:
		void ProcessFreeMove(float _deltaTime);

		ei::Mat4x4 m_projection;
		ei::Mat4x4 m_viewProjection;

		Mode m_mode;
	};

	extern Camera g_camera; // global default camera used for scene rendering
}