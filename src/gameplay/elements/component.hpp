#pragma once

#include <type_traits>
#include "../core/actor.hpp"

#define THISACTOR *static_cast<Actor*>(this)

namespace Game {

	class ActorComponent
	{
	public:
		ActorComponent(Actor& _owner) : m_actor(_owner) {}

		const Actor& GetActor() const { return m_actor; }

		void SetActive(bool _isActive) { m_isActive = _isActive; }
		bool IsActive() const { return m_isActive; }
	protected:
		Actor& m_actor;
		bool m_isActive; // should the component be processed (or rendered)
	};

	// render component that is drawn after lighting.
	class MarkerComponent : public ActorComponent
	{
	public:
		using ActorComponent::ActorComponent;

		virtual void Draw() = 0;
		virtual void ProcessComponent(float _deltaTime) {}
	};

	// post processing effects are order dependend
	class PostProcessComponent : public ActorComponent 
	{
	public:
		using ActorComponent::ActorComponent;

		virtual void Draw() = 0;
	};
}

// Use this cast if you want to access a specific components functions.
template<typename T, typename U>
T& component_cast(U& _component)
{
	static_assert(std::is_base_of_v<Game::ActorComponent, T>, "The target type is not an component type.");
	return *static_cast<T*>(&_component);
}