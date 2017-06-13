#pragma once

#include <vector>
#include <type_traits>
#include <memory>
#include <array>

#include "gameplay/elements/light.hpp"
#include "gameplay/elements/blackhole.hpp"
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
		template<typename T>
		void Add(T& _element)
		{
			static_assert(std::is_base_of<Actor, T>::value, "Only Actors can be managed by the SceneGraph.");
			// ownership is taken here
			m_actors.emplace_back(&_element);
			
			// Register components of T.
			RegisterGeometryComponent(_element);
			RegisterLightComponent(_element);
			RegisterPPComponent(_element);
			RegisterMarkerComponent(_element);
		}

		void Process(float _deltaTime);
		void Draw();

		void CleanUp();
	private:
		// If an object does not have the specified component
		// the empty register implementation is used.
		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<GeometryComponent, T>>>
		void RegisterGeometryComponent(T& _component) {}
		void RegisterGeometryComponent(GeometryComponent& _component);

		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<PostProcessComponent, T>>>
		void RegisterPPComponent(T& _component) {}
		void RegisterPPComponent(PostProcessComponent& _component);

		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<PointLightComponent, T>>>
		void RegisterLightComponent(T& _component) {}
		void RegisterLightComponent(PointLightComponent& _component);

		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<MarkerComponent, T>>>
		void RegisterMarkerComponent(T& _component) {}
		void RegisterMarkerComponent(MarkerComponent& _component);

		std::vector < std::unique_ptr<Actor>> m_actors;

		std::vector<PostProcessComponent*> m_postProcessComponents;
		std::vector<PointLightComponent*> m_lightComponents;
		std::vector<GeometryComponent*> m_geometryComponents;
		std::vector<MarkerComponent*> m_markerComponents;
	};
}