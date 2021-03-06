#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "lifetimecomponent.hpp"
#include "audiocomponent.hpp"
#include "gameplay/content.hpp"

namespace Game {

	// An explosion that deals damage and spawns a cloud of particles
	class Explosion : public Actor
	{
	public:
		Explosion(const ei::Vec3& _position, float _radius, float _damage, 
			Utils::Color8U _color = Utils::Color8U(0.7f, 0.8f, 0.4f, 1.f), 
			const std::string& _effectTex = "mist", 
			const clunk::Sample& _sound = Content::GetSound("explosion01"));

		void Process(float _deltaTime) override;

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		LifeTimeComponent m_lifeTimeComponent;
		PointLightComponent m_light;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_particles;
		AudioComponent m_audio;

		float m_radius;
	};

}