#pragma once

#include <vector>
#include <type_traits>
#include <memory>
#include <array>

#include "gameplay/core/model.hpp"
#include "core/actor.hpp"

namespace Game {

	/* SceneGraph *****************************************
	 * Handles actors that a scene is composed of.
	 * Manages both logic and drawing order.
	 */
	class SceneGraph
	{
	public:

		// Add an heap allocated object to the scene management.
		template<class T>
		void Add(T& _element)
		{
			static_assert(std::is_base_of<Actor, T>::value, "Only Actors can be managed by the SceneGraph.");

			// ownership is taken here
			m_actors.emplace_back(&_element);

			// let the actor register its own components
			_element.RegisterComponents(*this);
		}

		// register methods for specific components
		// The correct one will be selected by the compiler if the static type is supplied.
		// If the component does not require special treatment the least specific one is chosen.
		void RegisterComponent(ConstActorComponent& _component);
		void RegisterComponent(ActorComponent& _component);
		void RegisterComponent(class BaseParticleSystemComponent& _component);
		void RegisterComponent(GeometryComponent& _component);
		void RegisterComponent(class PointLightComponent& _component);
		void RegisterComponent(class PostProcessComponent& _component);
		void RegisterComponent(class MarkerComponent& _component);
		void RegisterComponent(class FactoryComponent& _component);

		void Process(float _deltaTime);
		void Draw();

		// removes all destroyed components and associated components.
		void CleanUp();

		// Registers by Factories created Actors tho the scene.
		void AddActors();
	private:
		std::vector < std::unique_ptr<Actor>> m_actors; // all active actors

		std::vector <ActorComponent*> m_actorComponents;
		std::vector<ConstActorComponent*> m_constActorComponents;
		std::vector<BaseParticleSystemComponent*> m_particleSystemComponents;

		std::vector<PostProcessComponent*> m_postProcessComponents;
		std::vector<PointLightComponent*> m_lightComponents;
		std::vector<GeometryComponent*> m_geometryComponents;
		std::vector<MarkerComponent*> m_markerComponents;
		std::vector<FactoryComponent*> m_factoryComponents;
	};
}