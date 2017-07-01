#include <algorithm>
#include "graphic/resources.hpp"
#include "linerenderer.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"

namespace Graphic {
	LineRenderer::LineRenderer(const Utils::Color32F& _color, float _thickness)
		: m_thickness(_thickness),
		m_color(_color),
		m_lineVertices(VertexArrayBuffer::PrimitiveType::LINE, { { VertexAttribute::VEC3,0 } }),
		m_numVertices(0)
	{}

	void LineRenderer::Add(const ei::Vec3& _begin, const ei::Vec3& _end)
	{
		auto vbGuard = m_lineVertices.GetBuffer(0);
		Vertex vertex;
		vertex.position = _begin;
		vbGuard->Add(vertex);
		vertex.position = _end;
		vbGuard->Add(vertex);

		m_numVertices += 2;
	}

	void LineRenderer::Remove()
	{
		auto vbGuard = m_lineVertices.GetBuffer(0);
		
		vbGuard->Remove<Vertex>(--m_numVertices);
		vbGuard->Remove<Vertex>(--m_numVertices);
	}

	void LineRenderer::Draw(const ei::Mat4x4& _worldViewProjection)
	{
		Device::SetEffect(Resources::GetEffect(Effects::LINES));
		
		UniformBuffer& ubo = Resources::GetUBO(UniformBuffers::OBJECT_LINES);
		ubo["c_WorldViewProjection"] = _worldViewProjection;
		ubo["c_Color"] = m_color;
		ubo["c_Thickness"] = m_thickness;

		Device::DrawVertices(m_lineVertices, 0, m_numVertices);
	}
}