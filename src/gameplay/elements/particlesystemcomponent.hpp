#pragma once

#include "graphic/effects/particlesystem.hpp"
#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "control/camera.hpp"

namespace Game {
	// Base type that can be used to identify a ParticleSystemComponent in compile time.
	class BaseParticleSystemComponent : public ConstActorComponent,
		public Graphic::ParticleSystems::SystemActions
	{
	public:
		BaseParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position, Graphic::ParticleSystems::RenderType _type);

		virtual void Draw() = 0;

	protected:
		ei::Vec3 m_positionOffset;
	};

	template<ei::uint PFlags>
	class ParticleSystemComponent : public BaseParticleSystemComponent,
		protected Graphic::ParticleSystems::System<PFlags>
	{
	public:
		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position, 
			Graphic::ParticleSystems::RenderType _type = Graphic::ParticleSystems::RenderType::BLOB)
			: BaseParticleSystemComponent(_actor, _position, _type),
			Graphic::ParticleSystems::System<PFlags>(m_particleVertices)
		{}

		void ProcessComponent(float _deltaTime) override
		{
			Simulate(_deltaTime);
		}

		// Add a particle 
		template<typename... Args>
		void AddParticle(const ei::Vec3& _position, const ei::Vec3& _velocity, Args&&... _args)
		{
			// transform particle into global space
			// only the actor has an extra rotation part
			ei::Vec3 position = m_actor.GetRotationMatrix() * (_position + m_positionOffset) + m_actor.GetPosition();
			ei::Vec3 velocity = m_actor.GetRotationMatrix() * _velocity;

			Graphic::ParticleSystems::System<PFlags>::AddParticle(position, velocity,
				std::forward<Args>(_args)...);
		}

		void Draw()
		{
			if (!GetNumParticles()) return;

			SystemActions::Draw(Control::g_camera.GetViewProjection(), GetNumParticles());
		}
	};

	template<ei::uint PFlags>
	using ParticleSystemActor = SingleComponentActor<ParticleSystemComponent<PFlags>>;
}