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
		SceneGraph();

		// Add an heap allocated object to the scene management.
		void Add(Actor& _element);

		// register methods for specific components
		// The correct one will be selected by the compiler if the static type is supplied.
		// If the component does not require special treatment the least specific one is chosen.
		void RegisterComponent(ConstActorComponent& _component);
		void RegisterComponent(ActorComponent& _component);
		void RegisterComponent(SceneComponent& _component);
		void RegisterComponent(class BaseParticleSystemComponent& _component);
		void RegisterComponent(GeometryComponent& _component);
		void RegisterComponent(class PointLightComponent& _component);
		void RegisterComponent(class PostProcessComponent& _component);
		void RegisterComponent(class MarkerComponent& _component);
		void RegisterComponent(class FactoryComponent& _component);
		void RegisterComponent(class CollisionComponent& _component);
		template<typename T, typename T1, typename T2>
		void RegisterComponent(CompositeComponent<T, T1, T2>& _component)
		{
			RegisterComponent(component_cast<T1>(_component));
			RegisterComponent(component_cast<T2>(_component));
		}
		template<typename T> 
		void RegisterComponent(DynamicComponent<T>& _component)
		{
			m_dynamicComponents.emplace_back(static_cast<DynamicComponentImpl*>(&_component));
			RegisterComponent(component_cast<T>(_component));
		}

		void Process(float _deltaTime, float _realdTime);
		void Draw();

		// removes all destroyed components and associated components.
		void CleanUp();
		// Registers Actors created by Factories to the scene.
		void AddActors();

		// Performs a ray cast on all objects in the scene.
		// @Return The closest object that is hit by the ray or nullptr if none is in range.
		Actor* RayCast(const ei::Ray& _ray, float _maxDist, CollisionFlags _type = CollisionComponent::Type::Any) const;

		// @Return All objects that have their center inside the given _sphere
		//	and their squared distance to the center.
		std::vector<std::pair<Actor*,float>> SphereQuery(const ei::Sphere& _sphere, CollisionFlags _flags) const;
	private:
		// Repairs the order of the internal CollisionComponent structure.
		// Only afterwards collision checks can be done.
		void SortAxis();

		void RegisterBaseComponent(ConstActorComponent& _component) { RegisterComponent(_component); }
		void RegisterBaseComponent(ActorComponent& _component) { RegisterComponent(_component); }

		void ResolveCollisions();

		std::vector < std::unique_ptr<Actor>> m_actors; // all active actors
		std::vector< std::unique_ptr<DynamicComponentImpl>> m_dynamicComponents;

		std::vector <ActorComponent*> m_actorComponents;
		std::vector<ConstActorComponent*> m_constActorComponents;
		std::vector<BaseParticleSystemComponent*> m_particleSystemComponents;
		std::vector<SceneComponent*> m_sceneComponents;

		std::vector<PostProcessComponent*> m_postProcessComponents;
		std::vector<PointLightComponent*> m_lightComponents;
		std::vector<GeometryComponent*> m_geometryComponents;
		std::vector<MarkerComponent*> m_markerComponents;
		std::vector<FactoryComponent*> m_factoryComponents;
		std::vector<CollisionComponent*> m_collisionComponents;
	};
}