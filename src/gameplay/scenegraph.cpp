#include "scenegraph.hpp"

#include "graphic/core/device.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/framebuffer.hpp"

using namespace Graphic;

namespace Game {

	void SceneGraph::Process(float _deltaTime)
	{
		// for now all actors are processed
		// this could be optimized by having an extra container for this
		for (auto& actor : m_actors)
			actor->Process(_deltaTime);
	}

	void SceneGraph::Draw()
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		// first step: render geometry to g-Buffer
		for (auto component : m_geometryComponents)
			component->Draw();

		// apply lights to the framebuffer
		for (auto component : m_lightComponents)
			component->Draw();
		Graphic::LightSystem::Draw();

		// render the framebuffer to the hardwarebackbuffer
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;

		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		// post processing reads from the framebuffer and writes to the backbuffer
		for (auto component : m_postProcessComponents)
			component->Draw();
	}

	void SceneGraph::RegisterGeometryComponent(GeometryComponent& _component)
	{
		m_geometryComponents.push_back(&_component);
	}

	void SceneGraph::RegisterLightComponent(PointLightComponent& _component)
	{
		m_lightComponents.push_back(&_component);
	}

	void SceneGraph::RegisterPPComponent(PostProcessComponent& _component)
	{
		m_postProcessComponents.push_back(&_component);
	}
}