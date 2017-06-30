#include "particlesystemcomponent.hpp"

namespace Game {
	BaseParticleSystemComponent::BaseParticleSystemComponent(const Actor& _actor, const ei::Vec3& _position)
		: ConstActorComponent(_actor),
		m_positionOffset(_position)
	{}
}