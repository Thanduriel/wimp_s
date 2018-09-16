#pragma once

#include "gameplay/core/component.hpp"

namespace Game {

	// Component that destroys the actor after its lifetime is over.
	class LifeTimeComponent : public ActorComponent
	{
	public:
		LifeTimeComponent(Actor& _actor, float _lifeTime, bool _shouldTriggerOnDestroy = true)
			: ActorComponent(_actor), 
			m_lifeTime(_lifeTime), 
			m_lifeTimeMax(_lifeTime),
			m_shouldTriggerOnDestroy(_shouldTriggerOnDestroy)
		{}

		LifeTimeComponent(Actor& _actor, const LifeTimeComponent& _orig)
			: ActorComponent(_actor), 
			m_lifeTime(_orig.m_lifeTime), 
			m_lifeTimeMax(_orig.m_lifeTimeMax),
			m_shouldTriggerOnDestroy(_orig.m_shouldTriggerOnDestroy)
		{}

		void ProcessComponent(float _deltaTime) override
		{
			m_lifeTime -= _deltaTime;

			if (m_lifeTime < 0.f) m_actor.Destroy(m_shouldTriggerOnDestroy);
		}

		float GetLifeTimeLeft() const { return m_lifeTime; }
		float GetLifeTimeMax() const { return m_lifeTimeMax; }
		float GetLifeTimeRelative() const { return m_lifeTime / m_lifeTimeMax; }
	private:
		bool m_shouldTriggerOnDestroy;
		float m_lifeTime;
		float m_lifeTimeMax;
	};
}