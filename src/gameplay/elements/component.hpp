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

	// Actor Component that does not change the state of the underlying actor.
	// Can be easily parallelized.
	class ConstActorComponent
	{
	public:
		ConstActorComponent(const Actor& _owner) : m_actor(_owner) {}

		const Actor& GetActor() const { return m_actor; }

		void SetActive(bool _isActive) { m_isActive = _isActive; }
		bool IsActive() const { return m_isActive; }
	protected:
		const Actor& m_actor;
		bool m_isActive;
	};

	namespace Details {
		template <typename T>
		class DynamicComponent : public T
		{
		public:
			virtual void OnRegister() {}
			virtual void ProcessComponent(float _deltaTime) {}
		};
	}

	// A component that can be added (or removed) at runtime to an actor.
	// Multiple instances of the same kind can be attached to the same actor.
	class ActorDynamicComponent : public ActorComponent
	{
	public:
		virtual void OnRegister() {}
		virtual void ProcessComponent(float _deltaTime) {}
	};
	// This is intentionally the same.
	// The additional const guaranty makes multi threading easier
	// but we do not need to much premature optimization right now.
	typedef ActorDynamicComponent DynamicConstActorComponent; //Details::DynamicComponent<ConstActorComponent>

	// render component that is drawn after lighting.
	class MarkerComponent : public ConstActorComponent
	{
	public:
		using ConstActorComponent::ConstActorComponent;

		virtual void Draw() = 0;
		virtual void ProcessComponent(float _deltaTime) {}
	};

	// post processing effects are order depended
	class PostProcessComponent : public ConstActorComponent 
	{
	public:
		using ConstActorComponent::ConstActorComponent;

		virtual void Draw() = 0;
	};
}

// Use this cast if you want to access a specific components functions.
template<typename T, typename U>
T& component_cast(U& _component)
{
	static_assert(std::is_base_of_v<Game::ActorComponent, T>
		|| std::is_base_of_v<Game::ConstActorComponent, T>, "The target type is not an component type.");
	return *static_cast<T*>(&_component);
}