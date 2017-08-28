#include "lightsystem.hpp"
#include "graphic/core/device.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/framebuffer.hpp"
#include "control/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"

#include "math/extensions.hpp"
#include <iostream>
namespace Graphic {
	Details::LightKey LightSystem::m_currentKey;
	VertexArrayBuffer* LightSystem::m_vertices;
	Details::LightInfoContainer LightSystem::m_lightInfos;

	LightInfo::LightInfo(float _radius, Utils::Color8U _color, float _intensity)
		: radius(_radius), color(_color), intensity(_intensity)
	{}

	LightInfo& LightHandle::operator*()
	{
		return m_reference[m_key];
	}

	const LightInfo& LightHandle::operator*() const
	{
		return m_reference[m_key];
	}

	LightInfo* LightHandle::operator->()
	{
		return &m_reference[m_key];
	}

	const LightInfo* LightHandle::operator->() const
	{
		return &m_reference[m_key];
	}

	// ************************************************************ //
	void LightSystem::Initialize()
	{
		m_vertices = new VertexArrayBuffer(VertexArrayBuffer::PrimitiveType::POINT,
		{ { VertexAttribute::VEC3, 0 },{ VertexAttribute::FLOAT, 1 },{ VertexAttribute::COLOR, 2 }, {VertexAttribute::FLOAT, 3} });
	}

	void LightSystem::Close()
	{
		delete m_vertices;
	}

	// ************************************************************ //
	LightHandle LightSystem::Register(const LightInfo& _info)
	{
		auto it = m_lightInfos.emplace(m_currentKey, _info);
		++m_currentKey;

		return LightHandle(m_lightInfos, it.first->first);
	}

	void LightSystem::DeRegister(const LightHandle& _hndl)
	{
		m_lightInfos.erase(_hndl.m_key);
	}

	// ************************************************************ //
	void LightSystem::Draw()
	{
		if (!m_lightInfos.size()) return;
		// update buffer
		size_t num = m_lightInfos.size();
		size_t i = 0;

		LightInfo* buffer = static_cast<LightInfo*>(malloc(num * sizeof(LightInfo)));
		for (auto& info : m_lightInfos)
		{
	/*		static int count = 0;
			++count;
			if (count % 2 && info.second.radius == 29.f)
			{
				std::cout << (Control::g_camera.GetView() * info.second.position).z << " | "
					<< (Control::g_camera.GetViewProjection() * info.second.position).z << std::endl;
			}*/
			buffer[i] = info.second;
			++i;
		}
		m_vertices->GetBuffer(0)->SetData((void*&)buffer, (int)num * sizeof(LightInfo));

		UniformBuffer& ubo = Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT);
		ubo["c_WorldViewProjection"] = Control::g_camera.GetViewProjection();

		Device::SetEffect(Resources::GetEffect(Effects::DEFFERED_LIGHT));
		Texture& colorTex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().front().pTexture;
		Device::SetTexture(colorTex, 0);
		const Texture& depthTex = *Device::GetCurrentFramebufferBinding()->GetDepthStencilAttachment().pTexture;
		Device::SetTexture(depthTex, 1);
		Texture& normalTex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().back().pTexture;
		Device::SetTexture(normalTex, 2);
		Device::DrawVertices(*m_vertices, 0, (int)num);
	}
}