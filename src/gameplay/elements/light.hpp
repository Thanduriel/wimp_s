#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "math/transformation.hpp"

namespace Game {
	class PointLightComponent : public ConstActorComponent, public Transformation
	{
	public:
		PointLightComponent(const Actor& _actor, const ei::Vec3& _position, float _radius, Utils::Color8U _color, float _intensity = 1.f);
		PointLightComponent(const Actor& _actor, const PointLightComponent& _orig);
		~PointLightComponent();

		void Draw();

		Graphic::LightHandle& GetLight() { return m_handle; }
	private:
		Graphic::LightHandle m_handle;
	};

	typedef SingleComponentActor<PointLightComponent> PointLight;
}