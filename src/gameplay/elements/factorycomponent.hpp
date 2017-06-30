#pragma once

#include <vector>
#include <memory>

#include "gameplay/core/actor.hpp"
#include "gameplay/core/component.hpp"

namespace Game {

	// Intern implementation of Scene registering.
	// could be subject to change.
	class SceneRegister
	{
	protected:
		// Add an actor to this functions internal container to be later registered by the scene.
		void Add(Actor& _actor) { m_createdActors.emplace_back(&_actor); };

		std::vector<std::unique_ptr<Actor>> m_createdActors;

		friend class SceneGraph;
	};

	class FactoryComponent : public SceneRegister, public ConstActorComponent
	{
		using ConstActorComponent::ConstActorComponent;
	public:
		/* Make *******************************************
		* Create an actor that will be registered by the Scene
		* @param _position The position of the actor to be created in the space of 
		*				this actors transformation.
		* @param _args Arguments forwarded to the constructor of T.
		*				! A rotation supplied through this will not be global.
						Use MakeR() for this instead.
		*/
		template<typename T, typename... Args>
		T& Make(const ei::Vec3& _position, Args&&... _args)
		{
			ei::Vec3 pos(m_actor.GetTransformation() * ei::Vec4(_position, 1));
			auto p = new T(pos, std::forward<Args>(_args)...);

			Add(*p);
			return *p;
		}

		/* MakeR *******************************************
		 * Create an actor that will be registered by the Scene.
		 * @param _position, _rotation in local space of this components actor.
		 * @param _args Further arguments forwarded to the constructor of T.
		 */
		template<typename T, typename... Args>
		void MakeR(const ei::Vec3& _position, const ei::Quaternion& _rot, Args&&... _args)
		{
			ei::Vec3 pos = m_actor.GetTransformation() * ei::Vec4(_position, 1);
			auto p = new T(pos, m_actor.GetRotation() * _rot, std::forward<Args>(_args)...);

			Add(p);
		}
	};
}