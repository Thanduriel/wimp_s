#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "gameplay/core/model.hpp"
#include "../lifetimecomponent.hpp"

namespace Game {

	class Projectile : public DynamicActor
	{
	public:
		Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _lifeTime);

		void OnDestroy() override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		LifeTimeComponent m_lifeTimeComponent;
	};

	class Rocket : public Projectile
	{
	public:
		Rocket(const ei::Vec3& _position, const ei::Vec3 _velocity, float _lifeTime);

		void Process(float _deltaTime) override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		float m_particleSpawnCount;

		GeometryComponent m_mesh;
		PointLightComponent m_engineLight;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_thrustParticles;
	};
}