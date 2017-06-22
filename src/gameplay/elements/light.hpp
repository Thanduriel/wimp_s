#pragma once

#include "gameplay/core/actor.hpp"
#include "component.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "math/transformation.hpp"

namespace Game {
	class PointLightComponent : public ActorComponent, public Transformation
	{
	public:
		PointLightComponent(Actor& _actor, const ei::Vec3& _position, float _radius, Utils::Color8U _color);
		~PointLightComponent();

		void Draw();
	private:
		Graphic::LightHandle m_handle;
	};

	class PointLight : public Actor, public PointLightComponent
	{
	public:
		PointLight(const ei::Vec3& _position, float _radius, Utils::Color8U _color);
	};
}