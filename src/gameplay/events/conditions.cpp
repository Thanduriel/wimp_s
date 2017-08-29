#include "conditions.hpp"
#include "event.hpp"
#include "utils/stringutils.hpp"

namespace Game {
namespace Conditions {
	OnDestroy::OnDestroy(Actor& _actor, std::vector<Actor::ConstHandle>&& _targets, int _numKills)
		: ActorComponent(_actor),
		m_targets(std::move(_targets)),
		m_numKillsRequired(_numKills)
	{}

	void OnDestroy::ProcessComponent(float _deltaTime)
	{
		// more likely case is that the goal is not met yet
		// assume that it is and stop once it is impossible
		int n = m_numKillsRequired - int(m_targets.size());
		for (auto& hndl : m_targets)
		{
			if (*hndl) ++n;
			// to many still alive
			if (n > 0)
				break;
		}

		if (n <= 0)
			m_actor.Destroy();
	}

	std::string OnDestroy::GetProgression() const
	{
		int n = 0;
		for (auto& hndl : m_targets)
			if (!*hndl) ++n;

		return std::to_string(n) + "/" + std::to_string(m_numKillsRequired);
	}

	// ********************************************************************* //
	Timer::Timer(Actor& _actor, float _waitTime)
		: ActorComponent(_actor),
		m_timeLeft(_waitTime)
	{
	}

	void Timer::ProcessComponent(float _deltaTime)
	{
		m_timeLeft -= _deltaTime;
		if (m_timeLeft <= 0.f) m_actor.Destroy();
	}

	std::string Timer::GetProgression() const
	{
		return Utils::ToFixPoint(m_timeLeft, 1);
	}

	// ********************************************************************* //
	IsClose::IsClose(Actor& _actor, const Actor& _target, const ei::Vec3& _position, float _maxDistance)
		: ActorComponent(_actor),
		m_target(_target),
		m_position(_position),
		m_maxDistSqr(_maxDistance * _maxDistance)
	{
	}

	void IsClose::ProcessComponent(float _deltaTime)
	{
		if (ei::lensq(m_target.GetPosition() - m_position) <= m_maxDistSqr)
			m_actor.Destroy();
	}

	std::string IsClose::GetProgression() const
	{
		return Utils::ToFixPoint(ei::len(m_target.GetPosition() - m_position), 1);
	}

	// ********************************************************************* //
	Composite::Composite(Actor& _actor, const EventCounter& _counter, int _targetNum)
		: ActorComponent(_actor),
		m_counter(_counter),
		m_targetNum(_targetNum)
	{}

	void Composite::ProcessComponent(float _deltaTime)
	{
		if (m_counter.num >= m_targetNum)
			m_actor.Destroy();
	}

	std::string Composite::GetProgression() const
	{
		return std::to_string(m_counter.num) + "/" + std::to_string(m_targetNum);
	}

}
}