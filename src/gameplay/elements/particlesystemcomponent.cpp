#include "particlesystemcomponent.hpp"

namespace Game {
	BaseParticleSystemComponent::BaseParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position, Graphic::ParticleSystems::RenderType _type)
		: ConstActorComponent(_actor),
		Transformation(_position),
		SystemActions(_type)
	{}
}