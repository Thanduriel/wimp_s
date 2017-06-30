#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/blackholerenderer.hpp"

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

	typedef SingleComponentActor<BlackHoleComponent> BlackHole;
}