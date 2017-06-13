#pragma once

#include "graphic/effects/linerenderer.hpp"
#include "gameplay/core/actor.hpp"
#include "component.hpp"
#include "utils/color.hpp"

namespace Game {

	class GridComponent : public MarkerComponent
	{
	public:
		GridComponent(Actor& _actor, const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f);

		void Draw() override;

	private:
		Graphic::LineRenderer m_lineRenderer;
	};

	class Grid : public Actor, public GridComponent
	{
	public:
		Grid(const ei::Vec3& _position, const ei::Quaternion& _rotation,
			const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f);
	};
}