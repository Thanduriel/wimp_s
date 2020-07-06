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
			BaseComponent(T _owner) : m_actor(_owner), m_canTick(true), m_isActive(true) {}

			// standard constructors do not work since they need to provide an owner
			BaseComponent() = delete;
			BaseComponent(const BaseComponent&) = delete;
			BaseComponent& operator=(const BaseComponent&) = delete;
			BaseComponent(BaseComponent&&) = delete;
			BaseComponent& operator=(BaseComponent&&) = delete;

			const T GetActor() const { return m_actor; }

			// Event called when the component is registered by the SceneGraph
			virtual void OnRegister() {}
			// Called every frame if m_canTick == true.
			virtual void ProcessComponent(float _deltaTime) {}

			// dynamically change whether the Component should be processed/drawn 
			void SetActive(bool _isActive) { m_isActive = _isActive; }
			bool IsActive() const { return m_isActive; }
			bool CanTick() const { return m_canTick; }
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

	class DynamicComponentImpl
	{
	public:
		DynamicComponentImpl(const Actor& _actor) : m_actor(_actor) {}
		virtual ~DynamicComponentImpl() {}

		const Actor& GetActor() const { return m_actor; }
	private:
		const Actor& m_actor;
	};

	// Mixin that allows component T to be added dynamically, that is after the Actor has been created.
	template<typename T>
	class DynamicComponent : public T, DynamicComponentImpl
	{
		static_assert(std::is_base_of_v<Game::ActorComponent, T>
			|| std::is_base_of_v<Game::ConstActorComponent, T>, "The target type is not an component type.");
	public:
		template<typename U, typename... Args>
		DynamicComponent(U& _actor, Args&&... _args)
			: T(_actor, std::forward<Args>(_args)...),
			DynamicComponentImpl(_actor)
		{}

	private:
		friend class SceneGraph; // the only thing that needs to access m_dynamicCompImpl
	};

	/* CompositeComponent **********************************************
	 * Mixin that makes handling a combination of components simpler.
	 * When registering a CompositeComponent both subcomponents are 
	 * automatically registered.
	 * Usage: 
	 *		class DoubleCompImpl : CompA, CompB {}; 
	 *		typedef CompositeComponent<DoubleCompImpl, CompA, CompB> DoubleComp;
	 */
	template<typename T, typename T1, typename T2>
	class CompositeComponent : public T
	{
	public:
		using T::T;
	};

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

	// A component that can perform queries on the SceneGraph.
	class SceneComponent :public ActorComponent
	{
	public:
		using ActorComponent::ActorComponent;

		// This function is called after regular processing and after the SceneGraph is sorted.
		virtual void ProcessComponent(float _deltaTime, const class SceneGraph& _sceneGraph) = 0;
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