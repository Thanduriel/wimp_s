#pragma once

#include "actor.hpp"
#include "component.hpp"
#include "gameplay/scenegraph.hpp"

namespace Game {

	/* SingleComponentActor *****************************
	 * Template to generate Actor-types with just one component attached to them.
	 * Handles management and creation of the component.
	 */
	template<class T>
	class SingleComponentActor : public Actor, public T
	{
	public:
		// Takes the position of the actor
		// and forwards all other arguments to the component.
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
		// simpler name for this type to perform constructor inheritance.
		typedef SingleComponentActor<T> SuperType;
	};
}