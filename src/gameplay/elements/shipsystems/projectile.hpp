#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "gameplay/core/model.hpp"
#include "../lifetimecomponent.hpp"

namespace Game {

	/* Projectile ***********************************************
	 * Basic projectile with limited life time and damage on hit.
	 */
	class Projectile : public DynamicActor
	{
	public:
		// projectile speed is constant for now
		// since hitting moving targets is near impossible otherwise
		static constexpr float DEFAULT_SPEED = 42.f;

		enum struct Visual {
			Simple,
			Rocket
		};

		Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity,
			float _damage, float _lifeTime);

		// does a correct copy of the given projectile including components
		Projectile(const Projectile& _proj);

		void OnDestroy() override;
		void OnCollision(Actor& _other) override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		float m_damage;

		LifeTimeComponent m_lifeTimeComponent;
	};

	class Bolt : public Projectile
	{
	public:
		Bolt(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime );
		Bolt(const Bolt& _orig);

		void Process(float _deltaTime) override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		CollisionComponent& GetCollisionComponent() { return m_collisionComponent; }
	private:
		ParticleSystemComponent<Graphic::PSComponent::POSITION
			| Graphic::PSComponent::COLOR 
			| Graphic::PSComponent::SIZE> m_particles;

		CollisionComponent m_collisionComponent;
	};

	/* Rocket **************************************************
	 * A projectile with acceleration and target-seeking functionality.
	 * Explodes on impact to deal aoe damage.
	 */
	class Rocket : public Projectile
	{
	public:
		static constexpr float DEFAULT_SPEED = 10.f;

		Rocket(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime = 10.f);
		Rocket(const Rocket& _orig);

		void Process(float _deltaTime) override;
		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		CollisionComponent& GetCollisionComponent() { return m_model; }
	private:
		float m_particleSpawnCount;

		ModelComponent m_model;
		PointLightComponent m_engineLight;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_thrustParticles;

		friend class WeaponTrait;
	};
}