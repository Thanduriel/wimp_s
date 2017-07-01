#include "scenegraph.hpp"

#include "graphic/core/device.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/framebuffer.hpp"

#include "gameplay/elements/light.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/particlesystemcomponent.hpp"
#include "elements/factorycomponent.hpp"

using namespace Graphic;

namespace Game {
	// thread safe instances
	std::vector<FactoryActor*> FactoryActor::m_instances;

	SceneGraph::SceneGraph()
	{
		// register general factories
		FactoryActor* factory = new FactoryActor(ei::Vec3(0.f));
		Add(*factory);
		FactoryActor::m_instances.push_back(factory);
	}

	void SceneGraph::Process(float _deltaTime)
	{
		// for now all actors are processed
		// this could be optimized by having an extra container for this
		for (auto& actor : m_actors)
			actor->Process(_deltaTime);

		for (auto component : m_constActorComponents)
			component->ProcessComponent(_deltaTime);
		for (auto component : m_actorComponents)
			component->ProcessComponent(_deltaTime);

		Graphic::ParticleSystems::Manager::Process(_deltaTime);

		for (auto component : m_markerComponents)
			component->ProcessComponent(_deltaTime);
	}

	void SceneGraph::Draw()
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		// first step: render geometry to g-Buffer
		for (auto component : m_geometryComponents)
			component->Draw();

		// particles can be illuminated too
		Graphic::ParticleSystems::Manager::Draw(Control::g_camera);

		// apply lights to the frame-buffer
		for (auto component : m_lightComponents)
			component->Draw();
		Graphic::LightSystem::Draw();

		// post lighting 3d effects
		for (auto component : m_markerComponents)
			component->Draw();

		// render the frame-buffer to the hardware back-buffer
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;

		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		// post processing reads from the frame-buffer and writes to the back-buffer
		for (auto component : m_postProcessComponents)
		component->Draw();
	}

	// *********************************************************** //
	void SceneGraph::RegisterComponent(ConstActorComponent& _component)
	{
		_component.OnRegister();
		m_constActorComponents.push_back(&_component);
	}
	void SceneGraph::RegisterComponent(ActorComponent& _component)
	{
		_component.OnRegister();
		m_actorComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(BaseParticleSystemComponent& _component)
	{
		RegisterComponent(component_cast<ConstActorComponent>(_component));
	}

	void SceneGraph::RegisterComponent(GeometryComponent& _component)
	{
		m_geometryComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(PointLightComponent& _component)
	{
		m_lightComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(PostProcessComponent& _component)
	{
		m_postProcessComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(MarkerComponent& _component)
	{
		m_markerComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(FactoryComponent& _component)
	{
		m_factoryComponents.push_back(&_component);
	}

	// *********************************************************** //
	template <typename T>
	void RemoveDestroyed(std::vector<T*>& _container)
	{
		auto it = std::remove_if(_container.begin(), _container.end(), [](const T* _component)
		{
			return _component->GetActor().IsDestroyed();
		});
		_container.erase(it, _container.end());
	}

	void SceneGraph::CleanUp()
	{
		// todo: test performance; parallelize or perform less frequent

		// unregister components
		RemoveDestroyed(m_geometryComponents);
		RemoveDestroyed(m_lightComponents);
		RemoveDestroyed(m_postProcessComponents);
		RemoveDestroyed(m_markerComponents);
		RemoveDestroyed(m_particleSystemComponents);
		RemoveDestroyed(m_factoryComponents);
		RemoveDestroyed(m_constActorComponents);
		RemoveDestroyed(m_actorComponents);

		// actual destruction is last
		auto it = std::remove_if(m_actors.begin(), m_actors.end(), [](const std::unique_ptr<Actor>& _actor)
		{
			return _actor->IsDestroyed();
		});
		m_actors.erase(it, m_actors.end());
	}

	void SceneGraph::AddActors()
	{
		// collect actors created by all known factories
		// it is possible that new factories are added, making any iterator invalid
		size_t size = m_factoryComponents.size();
		for (size_t i = 0; i < size; ++i)
		{
			auto& container = m_factoryComponents[i]->m_createdActors;
			while (container.size())
			{
				Add(*container.back().release());
				container.pop_back();
			}
		}
	}
}