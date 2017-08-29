#pragma once

#include <string>
#include "gameplay/core/component.hpp"
#include "gameplay/core/actor.hpp"

namespace Game {
	struct EventCounter;

namespace Conditions {

	// Has a group of Actors been destroyed?
	class OnDestroy : public ActorComponent
	{
	public:
		OnDestroy(Actor& _actor, std::vector<Actor::ConstHandle>&& _targets, int _numKillsRequired);

		void ProcessComponent(float _deltaTime) override;
		
		std::string GetProgression() const;
	private:
		const std::vector<Actor::ConstHandle> m_targets;
		const int m_numKillsRequired;
	};

	// ********************************************************************* //
	// Have some game time pass.
	class Timer : public ActorComponent
	{
	public:
		Timer(Actor& _actor, float _waitTime);

		void ProcessComponent(float _deltaTime) override;

		std::string GetProgression() const;
	private:
		float m_timeLeft;
	};

	// ********************************************************************* //
	// Reach the destination.
	class IsClose : public ActorComponent
	{
	public:
		// @param _target The Actor to track
		// @param _position The position _target must reach.
		IsClose(Actor& _actor, const Actor& _target, const ei::Vec3& _position, float _maxDistance);

		void ProcessComponent(float _deltaTime) override;

		std::string GetProgression() const;
	private:
		const Actor& m_target;
		ei::Vec3 m_position;
		float m_maxDistSqr;
	};

	// ********************************************************************* //
	// Have some of multiple events happen.
	// See event.hpp for usage instructions.
	class Composite : public ActorComponent
	{
	public:
		// @param _counter The EventCounter to watch
		// @param _targetNum Number of events that have to happen at least for this to trigger.
		Composite(Actor& _actor, const EventCounter& _counter, int _targetNum);

		void ProcessComponent(float _deltaTime) override;

		std::string GetProgression() const;
	private:
		const EventCounter& m_counter;
		int m_targetNum;
	};
}

}