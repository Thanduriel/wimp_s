#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "lifetimecomponent.hpp"

namespace Game {

	// An explosion that deals damage and spawns a cloud of particles
	class Explosion : public Actor
	{
	public:
		Explosion(const ei::Vec3& _position, float _radius, float _damage);

		void Process(float _deltaTime) override;

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		LifeTimeComponent m_lifeTimeComponent;
		PointLightComponent m_light;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_particles;

		float m_radius;
	};

}