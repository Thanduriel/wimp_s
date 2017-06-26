#pragma once

#include "actor.hpp"
#include "component.hpp"
#include "gameplay/scenegraph.hpp"

namespace Game {

	template<class T>
	class SingleComponentActor : public Actor, public T
	{
	public:
		template<typename... Args>
		SingleComponentActor(const ei::Vec3& _position, Args&&... _args)
			: Actor(_position),
			T(THISACTOR, std::forward<Args>(_args)...)
		{}

		void RegisterComponents(SceneGraph& _sceneGraph)
		{
			_sceneGraph.RegisterComponent(component_cast<T>(*this));
		}
	protected:
		typedef SingleComponentActor<T> SuperType;
	};
}