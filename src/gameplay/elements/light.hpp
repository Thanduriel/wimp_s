#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "math/transformation.hpp"

namespace Game {
	class PointLightComponent : public ActorComponent, public Transformation
	{
	public:
		PointLightComponent(Actor& _actor, const ei::Vec3& _position, float _radius, Utils::Color8U _color);
		~PointLightComponent();

		void Draw();

		Graphic::LightHandle& GetLight() { return m_handle; }
	private:
		Graphic::LightHandle m_handle;
	};

	typedef SingleComponentActor<PointLightComponent> PointLight;
}