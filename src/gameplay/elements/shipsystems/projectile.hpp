#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "gameplay/core/model.hpp"
#include "../lifetimecomponent.hpp"

namespace Game {

	/* Projectile ***********************************************
	 * Basic projectile with limited life time (and damage on hit)
	 */
	class Projectile : public DynamicActor
	{
	public:
		Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _lifeTime);

		void OnDestroy() override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		LifeTimeComponent m_lifeTimeComponent;
	};

	/* Rocket **************************************************
	 * A projectile with acceleration and target-seeking functionality.
	 * Explodes on impact to deal aoe damage.
	 */
	class Rocket : public Projectile
	{
	public:
		Rocket(const ei::Vec3& _position, const ei::Vec3& _velocity, float _lifeTime);

		void Process(float _deltaTime) override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		float m_particleSpawnCount;

		GeometryComponent m_mesh;
		PointLightComponent m_engineLight;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_thrustParticles;
	};
}