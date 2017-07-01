#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	// Component that destroys the actor after its lifetime is over.
	class LifeTimeComponent : public ActorComponent
	{
	public:
		LifeTimeComponent(Actor& _actor, float _lifeTime) : ActorComponent(_actor), m_lifeTime(_lifeTime) {}

		void ProcessComponent(float _deltaTime) override
		{
			m_lifeTime -= _deltaTime;

			if (m_lifeTime < 0.f) m_actor.Destroy();
		}
	private:
		float m_lifeTime;
	};
}