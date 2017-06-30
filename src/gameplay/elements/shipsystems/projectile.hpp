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
		GeometryComponent m_mesh;
		PointLightComponent m_engineLight;
		ParticleSystemComponent<Graphic::ParticleSystems::BASIC_SYSTEM> m_thrustParticles;
	};
}