#pragma once

#include "graphic/effects/particlesystem.hpp"
#include "component.hpp"
#include "math/transformation.hpp"
#include "gameplay/scenegraph.hpp"

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
	class ParticleSystemActor : public ParticleSystemComponent<PFlags>, public Actor
	{
	public:
		ParticleSystemActor(const ei::Vec3& _position, Graphic::ParticleSystems::RenderType _type)
			: ParticleSystemComponent<PFlags>(THISACTOR, _position, _type)
		{}

		void RegisterComponents(SceneGraph& _sceneGraph)
		{
			_sceneGraph.RegisterComponent(component_cast<BaseParticleSystemComponent>(*this));
		}
	};
}