#pragma once

#include "gameplay/core/actor.hpp"
#include "graphic/effects/blackholerenderer.hpp"
#include "component.hpp"

namespace Game
{
	class BlackHoleComponent : public PostProcessComponent
	{
	public:
		using PostProcessComponent::PostProcessComponent;

		void Draw() override;
	private:
		Graphic::BlackHoleRenderer m_renderer;
	};

	class BlackHole : public Actor, public BlackHoleComponent
	{
	public:
		BlackHole(const ei::Vec3& _position, float _radius);
	};
}