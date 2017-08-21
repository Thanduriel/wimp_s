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

	template<ei::uint PFlags, bool CanEmit = false>
	class ParticleSystemComponent : public BaseParticleSystemComponent
	{
	public:
		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position,
			Graphic::ParticleSystems::RenderType _type = Graphic::ParticleSystems::RenderType::BLOB)
			: BaseParticleSystemComponent(_actor, _position),
			m_system(Graphic::ParticleSystems::Manager::Get<PFlags>(_type))
		{
			m_canTick = CanEmit;
		}

		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position,
			const Graphic::Texture& _texture)
			: BaseParticleSystemComponent(_actor, _position),
			m_system(Graphic::ParticleSystems::Manager::Get<PFlags>(
				Graphic::ParticleSystems::RenderType::TEXQUAD,&_texture))
		{
			m_canTick = CanEmit;
		}

		ParticleSystemComponent(const Actor& _actor, const ParticleSystemComponent& _orig)
			: BaseParticleSystemComponent(_actor, _orig.m_positionOffset),
			m_system(_orig.m_system),
			m_generatorFunc(_orig.m_generatorFunc),
			m_spawnCount(_orig.m_spawnCount)
		{
			m_canTick = CanEmit;
		}


		// Add a managed particle to the system associated with this Component.
		// The arguments always follow the same order,
		// with ps-components that are not part of the system beeing skipped.
		template<typename... Args>
		void AddParticleRaw(Args&&... _args)
		{
			m_system.AddParticle(std::forward<Args>(_args)...);
		}

		// Adds a particle transforming its position
		// to world space from this Components space.
		template<typename... Args>
		void AddParticle(const ei::Vec3& _position, Args&&... _args)
		{
			ei::Vec3 position = m_actor.GetRotationMatrix() * (_position + m_positionOffset) + m_actor.GetPosition();
			m_system.AddParticle(position, std::forward<Args>(_args)...);
		}

		// Adds a particle transforming both position and velocity 
		// to world space from this Components space.
		template<typename... Args>
		void AddParticleV(const ei::Vec3& _position, const ei::Vec3& _velocity, Args&&... _args)
		{
			// transform particle into global space
			// only the actor has an extra rotation part
			ei::Vec3 position = m_actor.GetRotationMatrix() * (_position + m_positionOffset) + m_actor.GetPosition();
			ei::Vec3 velocity = m_actor.GetRotationMatrix() * _velocity;

			m_system.AddParticle(position, velocity,
				std::forward<Args>(_args)...);
		}

		template<typename... Args>
		void SetEmitter(float _numP, Args&&... _args)
		{
			Assert(CanEmit, "CanEmit has to be true to setup an Emitter.");
			m_output = _numP;
			m_spawnCount = 0.f;
			m_generatorFunc = [=]()
			{
				AddParticleV(_args()...);
			};
		}

		// Overwrite the number of particles that spawn every second.
		// Only useful when the Component CanEmit
		void SetOutput(float _numP)
		{
			Assert(CanEmit, "CanEmit has to be true to setup an Emitter.");
			m_output = _numP;
		}
		float GetOutput() const { return m_output; }

		void ProcessComponent(float _deltaTime) override
		{
//			Assert(m_generatorFunc, "A generator has to be setup with SetEmitter.");
			m_spawnCount += m_output * _deltaTime;
			while (m_spawnCount > 1.f)
			{
				--m_spawnCount;
				m_generatorFunc();
			}
		}
	private:
		Graphic::ParticleSystems::System<PFlags>& m_system;
		std::function<void()> m_generatorFunc;
		float m_spawnCount;
		float m_output;
	};

	template<ei::uint PFlags>
	using ParticleSystemActor = SingleComponentActor<ParticleSystemComponent<PFlags>>;
}