#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/sunrenderer.hpp"
#include "light.hpp"

namespace Game {

	class SunComponent : public MarkerComponent
	{
	public:
		SunComponent(const Actor& _actor, float _radius);

		void Draw() override;
	private:
		Graphic::SunRenderer m_renderer;
	};

	class Sun : public Actor
	{
	public:
		Sun(const ei::Vec3& _position, float _radius);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		SunComponent m_sunComponent;
	//	PointLightComponent m_lightComponent;
	};

//	typedef SingleComponentActor<SunComponent> Sun;
}