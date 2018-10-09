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
	public:
		// Add an actor to this functions internal container to be later registered by the scene.
		void Add(Actor& _actor) { m_createdActors.emplace_back(&_actor); };

		// Throw away all actors currently held by this SceneRegister
		void Reset() { m_createdActors.clear(); }
	protected:
		std::vector<std::unique_ptr<Actor>> m_createdActors;

		friend class SceneGraph;
		friend class Weapon; // hack for dps simulations
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
		T& MakeR(const ei::Vec3& _position, const ei::Quaternion& _rot, Args&&... _args)
		{
			ei::Vec3 pos = m_actor.GetTransformation() * ei::Vec4(_position, 1);
			
			return Make<T>(pos, m_actor.GetRotation() * _rot, std::forward<Args>(_args)...);
		}

		/* Copy ************************************************
		 * Creates a deep copy of the given prototype and registers it.
		 * Requires a copy constructor to exist.
		 */
		template <typename T>
		T& Copy(const T& _orig)
		{
			static_assert(std::is_copy_constructible_v<T>, "Copying requires a copy constructor to be available.");
			auto p = new T(_orig);
			Add(*p);
			return *p;
		}

		// Transforms the copy's position.
		template <typename T>
		T& CopyP(const T& _orig)
		{
			ei::Vec3 pos = ei::Vec3(m_actor.GetTransformation() * ei::Vec4(_orig.GetPosition(), 1.f));
			T& cpy = Copy<T>(_orig);
			cpy.SetPosition(pos);

			return cpy;
		}

		// Transforms the copy's position and rotation.
		template <typename T>
		T& CopyR(const T& _orig)
		{
			T& cpy = CopyP<T>(_orig);
			cpy.SetRotation(m_actor.GetRotation() * _orig.GetRotation());

			return cpy;
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