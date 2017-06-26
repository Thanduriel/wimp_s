#pragma once

#include "graphic/effects/particlesystem.hpp"
#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/core/singlecomponentactor.hpp"

namespace Game {
	// Base type that can be used to identify a ParticleSystemComponent in compile time.
	class BaseParticleSystemComponent : public ConstActorComponent, public Transformation,
		public Graphic::ParticleSystems::SystemActions
	{
	public:
		BaseParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position, Graphic::ParticleSystems::RenderType _type);

		virtual void Draw() = 0;
	};

	template<ei::uint PFlags>
	class ParticleSystemComponent : public BaseParticleSystemComponent,
		public Graphic::ParticleSystems::System<PFlags>
	{
	public:
		ParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position, Graphic::ParticleSystems::RenderType _type)
			: BaseParticleSystemComponent(_actor, _position, _type),
			Graphic::ParticleSystems::System<PFlags>(m_particleVertices)
		{}

		void ProcessComponent(float _deltaTime) override
		{
			Simulate(_deltaTime);
		}

		void Draw()
		{
			if (!GetNumParticles()) return;

			SystemActions::Draw(Control::g_camera.GetViewProjection() * Transformation::Get(m_actor.GetTransformation()), GetNumParticles());
		}
	};

	template<ei::uint PFlags>
	class ParticleSystemActor : public SingleComponentActor<ParticleSystemComponent<PFlags>>
	{
	public:
		using SuperType::SingleComponentActor;
	};
}