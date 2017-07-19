#pragma once

#include <functional>

#include "graphic/effects/particlesystem.hpp"
#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "control/camera.hpp"

namespace Game {
	// Base type that can be used to identify a ParticleSystemComponent in compile time.
	class BaseParticleSystemComponent : public ConstActorComponent
	{
	public:
		BaseParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position);

	protected:
		ei::Vec3 m_positionOffset;
	};

	template<ei::uint PFlags>
	class ParticleSystemComponent : public BaseParticleSystemComponent
	{
	public:
		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position,
			Graphic::ParticleSystems::RenderType _type = Graphic::ParticleSystems::RenderType::BLOB)
			: BaseParticleSystemComponent(_actor, _position),
			m_system(Graphic::ParticleSystems::Manager::Get<PFlags>(_type))
		{
			m_canTick = false;
		}

		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position,
			const Graphic::Texture& _texture)
			: BaseParticleSystemComponent(_actor, _position),
			m_system(Graphic::ParticleSystems::Manager::Get<PFlags>(
				Graphic::ParticleSystems::RenderType::TEXQUAD,&_texture))
		{
			m_canTick = false;
		}

		ParticleSystemComponent(const Actor& _actor, const ParticleSystemComponent& _orig)
			: BaseParticleSystemComponent(_actor, _orig.m_positionOffset),
			m_system(_orig.m_system) 
		{}


		// Add a managed particle to the system.
		// The arguments always follow the same order,
		// ps components that are not part of the system are skipped.
		template<typename... Args>
		void AddParticle(const ei::Vec3& _position, const ei::Vec3& _velocity, Args&&... _args)
		{
			// transform particle into global space
			// only the actor has an extra rotation part
			ei::Vec3 position = m_actor.GetRotationMatrix() * (_position + m_positionOffset) + m_actor.GetPosition();
			ei::Vec3 velocity = m_actor.GetRotationMatrix() * _velocity;

			m_system.AddParticle(position, velocity,
				std::forward<Args>(_args)...);
		}

		template<typename... Args>
		void AddParticle(const ei::Vec3& _position, Args&&... _args)
		{
			ei::Vec3 position = m_actor.GetRotationMatrix() * (_position + m_positionOffset) + m_actor.GetPosition();
			m_system.AddParticle(position, std::forward<Args>(_args)...);
		}

	private:
		Graphic::ParticleSystems::System<PFlags>& m_system;
		std::function<void(const ParticleSystemComponent&)> m_generatorFunc;
	};

	template<ei::uint PFlags>
	using ParticleSystemActor = SingleComponentActor<ParticleSystemComponent<PFlags>>;
}