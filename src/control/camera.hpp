#pragma once

#include "gameplay/core/actor.hpp"

namespace Control {
	class Camera : public Game::DynamicActor
	{
	public:
		enum struct Mode
		{
			Follow,
			Free,
			Tactical
		};

		Camera(const ei::Vec3& _position, const ei::Quaternion& _rotation, float _fov, float _aspectRatio);

		void Process(float _deltaTime);

		const ei::Mat4x4& GetViewProjection() const { return m_viewProjection; }
	private:
		void ProcessFreeMove(float _deltaTime);

		ei::Mat4x4 m_projection;
		ei::Mat4x4 m_viewProjection;

		Mode m_mode;
	};

	extern Camera g_camera; // global default camera used for scene rendering
}