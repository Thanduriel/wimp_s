#pragma once

#include "graphic/effects/linerenderer.hpp"
#include "gameplay/core/actor.hpp"
#include "utils/color.hpp"

namespace Game {
	class Grid : public Actor
	{
	public:
		Grid(const ei::Vec3& _position, const ei::Quaternion& _rotation,
			const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f);

		void Draw() override;
	private:
		Graphic::LineRenderer m_lineRenderer;
	};
}