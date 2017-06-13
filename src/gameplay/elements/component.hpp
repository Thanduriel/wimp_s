#pragma once

#include <type_traits>
#include "../core/actor.hpp"

#define THISACTOR *static_cast<Actor*>(this)

namespace Game {

	class ActorComponent
	{
	public:
		ActorComponent(Actor& _owner) : m_actor(_owner) {}
	protected:
		Actor& m_actor;
	};

/*	template<typename T>
	class GeometryComponent : public ActorComponent<T> {};

	template<typename T>
	class LightComponent : public ActorComponent<T> {};
	*/
	// post processing effects are order dependend
	class PostProcessComponent : public ActorComponent 
	{
	public:
		using ActorComponent::ActorComponent;

		virtual void Draw() = 0;
	};
}