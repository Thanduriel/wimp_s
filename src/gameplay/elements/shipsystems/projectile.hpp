#pragma once

#include "gameplay/elements/particlesystemcomponent.hpp"
#include "gameplay/elements/light.hpp"
#include "gameplay/core/model.hpp"

namespace Game {

	class Projectile : public DynamicActor
	{
	public:
		Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _lifeTime);

		void Process(float _deltaTime) override;
	private:
		float m_lifeTime; //< remaining life time
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

	class Explosion : public Actor
	{
	public:
		Explosion(const ei::Vec3& _position);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
		void Process(float _deltaTime) override;
	private:
		PointLightComponent m_light;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_particles;
		float m_lifeTime;
	};
}