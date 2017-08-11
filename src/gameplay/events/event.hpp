#pragma once

#include <vector>
#include <functional>
#include "gameplay/core/actor.hpp"

namespace Game {

	/* Event ***********************************************
	 * An event is the combination of a condition to be met and actions that are performed.
	 * @param T The conditions should be in the form of a Component that destroys the actor when met.
	 */
	template<typename T>
	class Event : public Actor, T
	{
	public:
		template<typename... Args>
		Event(std::function<void()>&& _actions, Args&&... _args)
			: Actor(),
			T(THISACTOR, std::forward<Args>(_args)...),
			m_actions(std::move(_actions))
		{}

		void RegisterComponents(class SceneGraph& _sceneGraph) override
		{
			_sceneGraph.RegisterComponent(*this);
		}

		void OnDestroy() override
		{
			m_actions();
		}

	private:
		std::function<void()> m_actions;
	};

	namespace Conditions
	{
		class Composite;
	}

	// These helpers can be used in combination with Condition::Composite.
	// to create boolean formulas instead of single conditions.
	// The individual conditions should be created as regular Events with the Action ActionIncrement.
	// All these Events require the same EventCounter.
	// A Composite can be used to check whether a minimum amount  of the pre conditions are met.
	struct EventCounter
	{
	private:
		friend struct ActionIncrement;
		friend class Conditions::Composite;
		int num = 0;
	};

	struct ActionIncrement
	{
		ActionIncrement(EventCounter& _eventCount) : m_eventCounter(_eventCount) {}

		void operator()() { ++m_eventCounter.num; }
	private:
		EventCounter& m_eventCounter;
	};
}