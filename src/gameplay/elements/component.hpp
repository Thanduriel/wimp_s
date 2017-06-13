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
	protected:
		Actor& m_actor;
	};

	// render component that is drawn after lighting.
	class MarkerComponent : public ActorComponent
	{
	public:
		using ActorComponent::ActorComponent;

		virtual void Draw() = 0;
	};

	// post processing effects are order dependend
	class PostProcessComponent : public ActorComponent 
	{
	public:
		using ActorComponent::ActorComponent;

		virtual void Draw() = 0;
	};
}