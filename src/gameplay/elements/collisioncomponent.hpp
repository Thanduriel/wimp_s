#pragma once

#include "gameplay/core/component.hpp"
#include "ei/3dtypes.hpp"

namespace Game {
	// While the collision component itself does not change the Actor
	// the solver needs to alter the Actors state through the component.
	class CollisionComponent : public ActorComponent
	{
	public:
		CollisionComponent(Actor& _actor, float _boundingRadius, const ei::Box& _boundingBox)
			: ActorComponent(_actor), 
			m_boundingRadius(_boundingRadius),
			m_boundingRadiusSq(_boundingRadius*_boundingRadius),
			m_boundingBox(_boundingBox)
		{
			ei::Vec3 v = _boundingBox.max - _boundingBox.min;
			m_volume = v.x * v.y * v.z;
		}

		float GetBoundingRadius() const { return m_boundingRadius; }
		float GetBoundingRadiusSq() const { return m_boundingRadiusSq; }
		const ei::Box& GetBoundingBox() const { return m_boundingBox; }
		float GetVolume() const { return m_volume; }
	private:
		float m_volume;
		float m_boundingRadius;
		float m_boundingRadiusSq;
		ei::Box m_boundingBox;
	};
}