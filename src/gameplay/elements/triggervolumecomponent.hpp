#pragma once

namespace Game {
	/* TriggerVolumeComponent ****************************************
	* A volume that invokes the given function on the closest Actor inside.
	* @param Fn A functor type with the signature: operator()(Actor& _self, Actor& _found)
	*/
	template<typename Fn>
	class TriggerVolumeComponent : public SceneComponent
	{
	public:
		TriggerVolumeComponent(Actor& _actor, float _radius, CollisionFlags _flags = CollisionComponent::Type::Ship)
			: SceneComponent(_actor),
			m_radius(_radius),
			m_flags(_flags)
		{
			m_canTick = false;
		}

		void ProcessComponent(float _deltaTime, const class SceneGraph& _sceneGraph) override
		{
			auto hits = _sceneGraph.SphereQuery(ei::Sphere(m_actor.GetPosition(), m_radius), m_flags);
			// a valid receiver is found
			if (hits.size())
			{
				// give items to the closest ship
				auto it = std::min_element(hits.begin(), hits.end(), [](const std::pair<Actor*, float>& _lhs, const std::pair<Actor*, float>& _rhs)
				{
					return _lhs.second < _rhs.second;
				});

				Fn fn;
				fn(m_actor, *it->first);
			}
		}
	private:
		float m_radius;
		CollisionFlags m_flags;
	};

}