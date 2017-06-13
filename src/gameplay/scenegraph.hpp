#pragma once

#include <vector>
#include <type_traits>
#include <memory>

#include "gameplay/elements/light.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/core/model.hpp"
#include "core/actor.hpp"

namespace Game {

	class GeometryComponent;
	class PointLightComponent;
	class PostProcessComponent;

	class SceneGraph
	{
	public:
		template<typename T>
		void Add(T& _element)
		{
			static_assert(std::is_base_of<Actor, T>::value, "Only Actors can be managed by the SceneGraph.");
			m_actors.emplace_back(&_element);

			RegisterGeometryComponent(_element);
			RegisterLightComponent(_element);
			RegisterPPComponent(_element);
		}

		void Process(float _deltaTime);
		void Draw();
	private:
		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<GeometryComponent, T>>>
		void RegisterGeometryComponent(T& _component) {}
		void RegisterGeometryComponent(GeometryComponent& _component);

		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<PostProcessComponent, T>>>
		void RegisterPPComponent(T& _component) {}
		void RegisterPPComponent(PostProcessComponent& _component);

		template<typename T, typename = std::enable_if_t<!std::is_base_of_v<PointLightComponent, T>>>
		void RegisterLightComponent(T& _component) {}
		void RegisterLightComponent(PointLightComponent& _component);

		std::vector < std::unique_ptr<Actor>> m_actors;

		std::vector<class PostProcessComponent*> m_postProcessComponents;
		std::vector<class PointLightComponent*> m_lightComponents;
		std::vector<class GeometryComponent*> m_geometryComponents;
	};
}