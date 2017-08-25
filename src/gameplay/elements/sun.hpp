#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/sunrenderer.hpp"

namespace Game {

	class SunComponent : public MarkerComponent
	{
	public:
		SunComponent(const Actor& _actor, float _radius);

		void Draw() override;
	private:
		Graphic::SunRenderer m_renderer;
	};

	typedef SingleComponentActor<SunComponent> Sun;
}