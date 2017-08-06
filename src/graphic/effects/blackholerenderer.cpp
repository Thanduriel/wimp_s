#include "blackholerenderer.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/framebuffer.hpp"

namespace Graphic {
	struct Vertex
	{
		ei::Vec3 position;
		float radius;
	};

	BlackHoleRenderer::BlackHoleRenderer()
		: m_vertices(VertexArrayBuffer::PrimitiveType::POINT, { { VertexAttribute::VEC3,0 }, 
		{VertexAttribute::FLOAT, 1} })
	{

	}

	void BlackHoleRenderer::Add(const ei::Vec3& _position, float _radius)
	{
		// temporary refresh
		auto vbGuard = m_vertices.GetBuffer(0);
		vbGuard->Clear();
		Vertex vertex;
		vertex.position = _position;
		vertex.radius = _radius;
		
		vbGuard->Add(vertex);
	}

	void BlackHoleRenderer::Draw(const ei::Mat4x4& _worldViewProjection)
	{
		Texture& tex = *Device::GetLastFrameBufferBinding()->GetColorAttachments().begin()->pTexture;
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::BLACKHOLE));
		Device::SetTexture(tex, 0);

		UniformBuffer& ubo = Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT);
		ubo["c_WorldViewProjection"] = _worldViewProjection;

		Device::DrawVertices(m_vertices, 0, m_vertices.GetNumVertices());
	}
}