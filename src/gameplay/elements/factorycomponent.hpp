#pragma once

#include <vector>
#include <memory>

#include "gameplay/core/actor.hpp"
#include "gameplay/core/component.hpp"
#include "gameplay/core/singlecomponentactor.hpp"

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

		template<typename T, typename... Args>
		T& Make(Args&&... _args)
		{
			auto p = new T(std::forward<Args>(_args)...);

			Add(*p);
			return *p;
		}

		/* Make *******************************************
		* Create an actor that will be registered by the Scene
		* @param _position The position of the actor to be created in the space of 
		*				this actors transformation.
		* @param _args Arguments forwarded to the constructor of T.
		*				! A rotation supplied through this will not be global.
						Use MakeR() for this instead.
		*/
		template<typename T, typename... Args>
		T& MakeP(const ei::Vec3& _position, Args&&... _args)
		{
			ei::Vec3 pos(m_actor.GetTransformation() * ei::Vec4(_position, 1.f));

			return Make<T>(pos, std::forward<Args>(_args)...);
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
			
			return Make<T>(pos, m_actor.GetRotation() * _rot, std::forward<Args>(_args)...);
		}
	};

	class FactoryActor : public SingleComponentActor<FactoryComponent>
	{
	public:
		using SuperType::SingleComponentActor;

		// Returns a thread safe FactoryActor that is registered in the scene.
		// Can be used to dynamically create Actors.
		static FactoryActor& GetThreadLocalInstance()
		{
			return *m_instances[0];
		}
	private:
		// currently defined in SceneGraph
		static std::vector<FactoryActor*> m_instances;
		friend class SceneGraph;
	};

	
}