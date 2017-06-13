#pragma once

#include "gameplay/core/actor.hpp"
#include "component.hpp"
#include "graphic/effects/lightsystem.hpp"

namespace Game {
	class PointLightComponent : public ActorComponent
	{
	public:
		PointLightComponent(Actor& _actor, float _radius, Utils::Color8U _color);
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