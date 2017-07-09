#pragma once

#include <type_traits>
#include "../core/actor.hpp"

// passes the actual actor instance to the constructor of an actor
#define THISACTOR *static_cast<Actor*>(this)

namespace Game {
	/* ActorComponent ***************************o
	 * A functional unit that implements some behavior or visual effect
	 * that can be added to an actor.
	 */
	namespace Details {
		template<typename T>
		class BaseComponent
		{
		public:
			BaseComponent(T _owner) : m_actor(_owner), m_canTick(true) {}

			// standard constructors do not work since they need to provide an owner
			/*
			BaseComponent() = delete;
			BaseComponent(const BaseComponent&) = delete;
			BaseComponent(BaseComponent&&) = delete;
			BaseComponent& operator=(const BaseComponent&) = delete;
			BaseComponent& operator=(BaseComponent&&) = delete;
			*/
			const T GetActor() const { return m_actor; }

			virtual void OnRegister() {}
			virtual void ProcessComponent(float _deltaTime) {}

			// does nothing currently
			void SetActive(bool _isActive) { m_isActive = _isActive; }
			bool IsActive() const { return m_isActive; }
		protected:
			T m_actor;
			bool m_isActive; // should the component be processed (or rendered)

			// Should this components ProcessComponent be called?
			// Can only be changed before registering.
			bool m_canTick;

			friend class SceneGraph;
		};
	}

	class ActorComponent : public Details::BaseComponent<Actor&> { using Details::BaseComponent<Actor&>::BaseComponent; };

	// Actor Component that does not change the state of the owning actor.
	// Can be easily parallelized. Use this as base class if possible.
	class ConstActorComponent : public Details::BaseComponent<const Actor&> { using Details::BaseComponent<const Actor&>::BaseComponent; };

	// A render component that is drawn after lighting
	// to display 3d structures that are not part of the world and should thus not be illuminated.
	class MarkerComponent : public ConstActorComponent
	{
	public:
		using ConstActorComponent::ConstActorComponent;

		virtual void Draw() = 0;
		virtual void ProcessComponent(float _deltaTime) {}
	};

	// Post processing effects are order depended and are drawn last.
	// The graphics pipeline is set up to read from the frame buffer of previous
	// steps and override the finished scene in the backbuffer.
	class PostProcessComponent : public ConstActorComponent 
	{
	public:
		using ConstActorComponent::ConstActorComponent;

		virtual void Draw() = 0;
	};
}

// Casts an Actor that consists of components by inheritance to the desired component.
// Use this cast if you want to access a specific components functions that would otherwise clash.
template<typename T, typename U>
T& component_cast(U& _component)
{
	static_assert(std::is_base_of_v<Game::ActorComponent, T>
		|| std::is_base_of_v<Game::ConstActorComponent, T>, "The target type is not an component type.");
	return *static_cast<T*>(&_component);
}