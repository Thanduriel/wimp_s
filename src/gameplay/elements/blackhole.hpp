#pragma once

#include "gameplay/core/actor.hpp"
#include "graphic/effects/blackholerenderer.hpp"

namespace Game
{
	class BlackHole : public Actor
	{
	public:
		using Actor::Actor;

		void Draw() override;
	private:
		Graphic::BlackHoleRenderer m_renderer;
	};
}