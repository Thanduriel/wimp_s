#pragma once

#include "gameplay/core/actor.hpp"
#include "graphic/effects/lightsystem.hpp"

namespace Game {
	class PointLight : public Actor
	{
	public:
		PointLight(const ei::Vec3 _position, float _radius, Utils::Color8U _color);
		~PointLight();

		void Process(float _deltaTime) override;
	private:
		Graphic::LightHandle m_handle;
	};
}