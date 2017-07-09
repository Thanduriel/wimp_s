#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	// Component that destroys the actor after its lifetime is over.
	class LifeTimeComponent : public ActorComponent
	{
	public:
		LifeTimeComponent(Actor& _actor, float _lifeTime) 
			: ActorComponent(_actor), m_lifeTime(_lifeTime), m_lifeTimeMax(_lifeTime) {}

		LifeTimeComponent(Actor& _actor, const LifeTimeComponent& _orig)
			: ActorComponent(_actor), m_lifeTime(_orig.m_lifeTime), m_lifeTimeMax(_orig.m_lifeTimeMax) {}

		void ProcessComponent(float _deltaTime) override
		{
			m_lifeTime -= _deltaTime;

			if (m_lifeTime < 0.f) m_actor.Destroy();
		}

		float GetLifeTimeLeft() const { return m_lifeTime; }
		float GetLifeTimeMax() const { return m_lifeTimeMax; }
	private:
		float m_lifeTime;
		float m_lifeTimeMax;
	};
}